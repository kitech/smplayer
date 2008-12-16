/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2008 Ricardo Villalba <rvm@escomposlinux.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "videopreview.h"
#include "videopreviewconfigdialog.h"
#include <QProcess>
#include <QRegExp>
#include <QDir>
#include <QTime>
#include <QProgressDialog>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>

// Workaround for Windows
#ifdef Q_OS_WIN
#define CD_TO_TEMP_DIR 1
#endif

VideoPreview::VideoPreview(QString mplayer_path, QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	mplayer_bin = mplayer_path;
	QFileInfo fi(mplayer_bin);
	if (fi.exists() && fi.isExecutable() && !fi.isDir()) {
		mplayer_bin = fi.absoluteFilePath();
    }

	qDebug("VideoPreview::VideoPreview: mplayer_bin: '%s'", mplayer_bin.toUtf8().constData());

	set = 0; // settings

	input_video.clear();
	n_cols = 4;
	n_rows = 4;
	initial_step = 20;
	max_width = 800;
	aspect_ratio = 0;
	display_osd = true;

	output_dir = "smplayer_preview";
	full_output_dir = QDir::tempPath() +"/"+ output_dir;

	progress = new QProgressDialog(this);
	progress->setWindowTitle(tr("Video preview"));
	progress->setCancelButtonText( tr("Cancel") );
	connect( progress, SIGNAL(canceled()), this, SLOT(cancelPressed()) );

	w_contents = new QWidget(this);
	QPalette p = w_contents->palette();
	p.setColor(w_contents->backgroundRole(), Qt::white);
	w_contents->setPalette(p);

	info = new QLabel(this);

	foot = new QLabel(this);
	foot->setAlignment(Qt::AlignRight);
	foot->setText("<i>"+ tr("Generated by SMPlayer") +" (http://smplayer.sf.net)</i>");

	grid_layout = new QGridLayout;
	grid_layout->setSpacing(2);

	QVBoxLayout * l = new QVBoxLayout;
	l->setSizeConstraint(QLayout::SetFixedSize);
	l->addWidget(info);
	l->addLayout(grid_layout);
	l->addWidget(foot);
	
	w_contents->setLayout(l);

	scroll_area = new QScrollArea(this);
	scroll_area->setWidgetResizable(true);
	scroll_area->setAlignment(Qt::AlignCenter);
	scroll_area->setWidget( w_contents );

	QDialogButtonBox * button_box = new QDialogButtonBox(QDialogButtonBox::Close | QDialogButtonBox::Save, Qt::Horizontal, this);
	connect( button_box, SIGNAL(rejected()), this, SLOT(close()) );
	connect( button_box->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(saveImage()) );

	QVBoxLayout * my_layout = new QVBoxLayout;
	my_layout->addWidget(scroll_area);
	my_layout->addWidget(button_box);
	setLayout(my_layout);
}

VideoPreview::~VideoPreview() {
	if (set) saveSettings();
}

void VideoPreview::setSettings(QSettings * settings) {
	set = settings;
	loadSettings();
}

void VideoPreview::clearThumbnails() {
	for (int n=0; n < label_list.count(); n++) {
		grid_layout->removeWidget( label_list[n] );
		delete label_list[n];
	}
	label_list.clear();
	info->clear();
}

bool VideoPreview::createThumbnails() {
	clearThumbnails();
	error_message.clear();

	bool result = extractImages();

	if ((result == false) && (!error_message.isEmpty())) {
		QMessageBox::critical(this, tr("Error"), 
                              tr("The following error has occurred while creating the thumbnails:")+"\n"+ error_message );
	}

	// Adjust size
	//resize( w_contents->sizeHint() );

	cleanDir(full_output_dir);
	return result;
}

void VideoPreview::adjustWindowSize() {
	qDebug("VideoPreview::adjustWindowSize: window size: %d %d", width(), height());
	qDebug("VideoPreview::adjustWindowSize: scroll_area size: %d %d", scroll_area->width(), scroll_area->height());

	int diff_width = width() - scroll_area->width() + 4; // Where does the hell this 4 come from?
	int diff_height = height() - scroll_area->height() + 4;

	qDebug("VideoPreview::adjustWindowSize: diff_width: %d diff_height: %d", diff_width, diff_height);

	QSize new_size = w_contents->size() + QSize( diff_width, diff_height);

	qDebug("VideoPreview::adjustWindowSize: new_size: %d %d", new_size.width(), new_size.height());

	resize(new_size);
}

bool VideoPreview::extractImages() {
	VideoInfo i = getInfo(mplayer_bin, input_video);
	int length = i.length;

	if (length == 0) {
		if (error_message.isEmpty()) error_message = tr("The length of the video is 0");
		return false;
	}

	// Create a temporary directory
	QDir d(QDir::tempPath());
	if (!d.exists(output_dir)) {
		if (!d.mkpath(output_dir)) {
			qDebug("VideoPreview::extractImages: error: can't create '%s'", full_output_dir.toUtf8().constData());
			error_message = tr("The temporary directory (%1) can't be created").arg(full_output_dir);
			return false;
		}
	}

	// Display info about the video
	QTime t = QTime().addSecs(i.length);
	info->setText(
		"<b><font size=+1>" + i.filename +"</font></b>"
		"<table cellspacing=4 cellpadding=4><tr>"
		"<td>" +
		tr("Size: %1 MB").arg(i.size / (1024*1024)) + "<br>" +
		tr("Resolution: %1x%2").arg(i.width).arg(i.height) + "<br>" +
		tr("Length: %1").arg(t.toString("hh:mm:ss")) +
		"</td>"
		"<td>" +
		tr("Video format: %1").arg(i.video_format) + "<br>" +
		tr("Frames per second: %1").arg(i.fps) + "<br>" +
		tr("Aspect ratio: %1").arg(i.aspect) + //"<br>" +
		"</td>"
		"<td>" +
		tr("Video bitrate: %1 kbps").arg(i.video_bitrate/1000) + "<br>" +
		tr("Audio bitrate: %1 kbps").arg(i.audio_bitrate/1000) + "<br>" +
		tr("Audio rate: %1 Hz").arg(i.audio_rate) + //"<br>" +
		"</td>"
		"</tr></table>" 
	);
	setWindowTitle( tr("Video preview") + " - " + i.filename );

	// Let's begin
	thumbnail_width = 0;

	int num_pictures = n_cols * n_rows;
	length -= initial_step;
	int s_step = length / num_pictures;

	int current_time = initial_step;

	canceled = false;
	progress->setLabelText(tr("Creating thumbnails..."));
	progress->setRange(1, num_pictures);

	int current_col = 0;
	int current_row = 0;
	for (int n=1; n <= num_pictures; n++) {
		qDebug("VideoPreview::extractImages: getting frame %d of %d...", n, num_pictures);
		progress->setValue(n);
		qApp->processEvents();

		if (canceled) return false;

		QStringList args;
		args << "-nosound" << "-vo" 
#ifdef CD_TO_TEMP_DIR
			<< "jpeg"
#else
			<< "jpeg:outdir="+full_output_dir
#endif
			<< "-frames" << "6"
            << "-ss" << QString::number(current_time);

		if (aspect_ratio != 0) {
			args << "-aspect" << QString::number(aspect_ratio) << "-zoom";
		}

		/*
		if (display_osd) {
			args << "-vf" << "expand=osd=1" << "-osdlevel" << "2";
		}
		*/

		args << input_video;

		QString command = mplayer_bin + " ";
		for (int n = 0; n < args.count(); n++) command = command + args[n] + " ";
		qDebug("VideoPreview::extractImages: command: %s", command.toUtf8().constData());

		QProcess p;
#ifdef CD_TO_TEMP_DIR
		p.setWorkingDirectory(full_output_dir);
		qDebug("VideoPreview::extractImages: changing working directory of the process to '%s'", full_output_dir.toUtf8().constData());
#endif
		p.start(mplayer_bin, args);
		if (!p.waitForFinished()) {
			qDebug("VideoPreview::extractImages: error running process");
			error_message = tr("The mplayer process didn't run");
			return false;
		}

		if (!QFile::exists(full_output_dir + "/00000005.jpg")) {
			error_message = tr("The file %1 doesn't exist").arg(full_output_dir + "/00000005.jpg");
			return false;
		}

		QString output_file = output_dir + QString("/picture_%1.jpg").arg(current_time, 8, 10, QLatin1Char('0'));
		d.rename(output_dir + "/00000005.jpg", output_file);

		if (!addPicture(QDir::tempPath() +"/"+ output_file, current_row, current_col, current_time)) {
			return false;
		}
		current_col++;
		if (current_col >= n_cols) { current_col = 0; current_row++; }

		current_time += s_step;
	}

	return true;
}

bool VideoPreview::addPicture(const QString & filename, int row, int col, int time) {
	qDebug("VideoPreview::addPicture: row: %d col: %d, file: '%s'", row, col, filename.toUtf8().constData());

	QPixmap picture;
	if (!picture.load(filename)) {
		qDebug("VideoPreview::addPicture: can't load file");
		error_message = tr("The file %1 can't be loaded").arg(filename);
		return false;
	}

	if (thumbnail_width == 0) {
		int spacing = grid_layout->horizontalSpacing() * (n_cols-1);
		if (spacing < 0) spacing = 0;
		qDebug("VideoPreview::addPicture: spacing: %d", spacing);
		thumbnail_width = (max_width - spacing) / n_cols;
		if (thumbnail_width > picture.width()) thumbnail_width = picture.width();
		qDebug("VideoPreview::addPicture: thumbnail_width set to %d", thumbnail_width);
	}

	QPixmap scaled_picture = picture.scaledToWidth(thumbnail_width, Qt::SmoothTransformation);

	// Add current time text
	if (display_osd) {
		QString stime = QTime().addSecs(time).toString("hh:mm:ss");
		QFont font("Arial");
		font.setBold(true);
		QPainter painter(&scaled_picture);
		painter.setPen( Qt::white );
		painter.setFont(font);
		painter.drawText(scaled_picture.rect(), Qt::AlignRight | Qt::AlignBottom, stime);
	}

	QLabel * l = new QLabel(this);
	label_list.append(l);
	l->setPixmap(scaled_picture);
	//l->setPixmap(picture);
	grid_layout->addWidget(l, row, col);

	return true;
}

void VideoPreview::cleanDir(QString directory) {
	QDir d(directory);
	QStringList l = d.entryList( QStringList() << "*.jpg", QDir::Files, QDir::Unsorted);

	for (int n = 0; n < l.count(); n++) {
		qDebug("VideoPreview::cleanDir: deleting '%s'", l[n].toUtf8().constData());
		d.remove(l[n]);
	}
	qDebug("VideoPreview::cleanDir: removing directory '%s'", directory.toUtf8().constData());
	d.rmpath(directory);
}

VideoInfo VideoPreview::getInfo(const QString & mplayer_path, const QString & filename) {
	VideoInfo i;

	if (filename.isEmpty()) {
		error_message = tr("No filename");
		return i;
	}

	QFileInfo fi(filename);
	if (fi.exists()) {
		i.filename = fi.fileName();
		i.size = fi.size();
	}

	QRegExp rx("^ID_(.*)=(.*)");

	QProcess p;
	p.setProcessChannelMode( QProcess::MergedChannels );

	QStringList args;
	args << "-vo" << "null" << "-ao" << "null" << "-frames" << "1" << "-identify" << "-nocache" << "-noquiet" << filename;
	p.start(mplayer_path, args);

	if (p.waitForFinished()) {
		QByteArray line;
		while (p.canReadLine()) {
			line = p.readLine().trimmed();
			qDebug("VideoPreview::getInfo: '%s'", line.constData());
			if (rx.indexIn(line) > -1) {
				QString tag = rx.cap(1);
				QString value = rx.cap(2);
				qDebug("VideoPreview::getInfo: tag: '%s', value: '%s'", tag.toUtf8().constData(), value.toUtf8().constData());

				if (tag == "LENGTH") i.length = (int) value.toDouble();
				else
				if (tag == "VIDEO_WIDTH") i.width = value.toInt();
				else
				if (tag == "VIDEO_HEIGHT") i.height = value.toInt();
				else
				if (tag == "VIDEO_FPS") i.fps = value.toDouble();
				else
				if (tag == "VIDEO_ASPECT") {
					i.aspect = value.toDouble();
					if ((i.aspect == 0) && (i.width != 0) && (i.height != 0)) {
						i.aspect = (double) i.width / i.height;
					}
				}
				else
				if (tag == "VIDEO_BITRATE") i.video_bitrate = value.toInt();
				else
				if (tag == "AUDIO_BITRATE") i.audio_bitrate = value.toInt();
				else
				if (tag == "AUDIO_RATE") i.audio_rate = value.toInt();
				else
				if (tag == "VIDEO_FORMAT") i.video_format = value;
			}
		}
	} else {
		qDebug("VideoPreview::getInfo: error: process didn't start");
		error_message = tr("The mplayer process didn't start while trying to get info about the video");
	}

	qDebug("VideoPreview::getInfo: filename: '%s'", i.filename.toUtf8().constData());
	qDebug("VideoPreview::getInfo: resolution: '%d x %d'", i.width, i.height);
	qDebug("VideoPreview::getInfo: length: '%d'", i.length);
	qDebug("VideoPreview::getInfo: size: '%d'", (int) i.size);

	return i;
}

void VideoPreview::cancelPressed() {
	canceled = true;
}

void VideoPreview::saveImage() {
	qDebug("VideoPreview::saveImage");

	QString proposed_name = last_directory;
	QFileInfo fi(input_video);
	if (fi.exists()) {
		proposed_name += "/"+fi.completeBaseName()+".jpg";
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),
                            proposed_name, tr("Images (*.png *.jpg)"));

	if (!filename.isEmpty()) {
		QPixmap image = QPixmap::grabWidget(w_contents);
		if (!image.save(filename)) {
			// Failed!!!
			qDebug("VideoPreview::saveImage: error saving '%s'", filename.toUtf8().constData());
			QMessageBox::warning(this, tr("Error saving file"), 
                                 tr("The file couldn't be saved") );
		} else {
			last_directory = QFileInfo(filename).absolutePath();
		}
	}
}

bool VideoPreview::showConfigDialog() {
	VideoPreviewConfigDialog d(this);

	d.setVideoFile( videoFile() );
	d.setCols( cols() );
	d.setRows( rows() );
	d.setInitialStep( initialStep() );
	d.setMaxWidth( maxWidth() );
	d.setDisplayOSD( displayOSD() );
	d.setAspectRatio( aspectRatio() );

	if (d.exec() == QDialog::Accepted) {
		setVideoFile( d.videoFile() );
		setCols( d.cols() );
		setRows( d.rows() );
		setInitialStep( d.initialStep() );
		setMaxWidth( d.maxWidth() );
		setDisplayOSD( d.displayOSD() );
		setAspectRatio( d.aspectRatio() );

		return true;
	}

	return false;
}

void VideoPreview::saveSettings() {
	qDebug("VideoPreview::saveSettings");

	set->beginGroup("videopreview");

	set->setValue("columns", cols());
	set->setValue("rows", rows());
	set->setValue("initial_step", initialStep());
	set->setValue("max_width", maxWidth());
	set->setValue("osd", displayOSD());
	set->setValue("last_directory", last_directory);
	set->setValue("filename", videoFile());

	set->endGroup();
}

void VideoPreview::loadSettings() {
	qDebug("VideoPreview::loadSettings");

	set->beginGroup("videopreview");

	setCols( set->value("columns", cols()).toInt() );
	setRows( set->value("rows", rows()).toInt() );
	setInitialStep( set->value("initial_step", initialStep()).toInt() );
	setMaxWidth( set->value("max_width", maxWidth()).toInt() );
	setDisplayOSD( set->value("osd", displayOSD()).toBool() );
	last_directory = set->value("last_directory", last_directory).toString();
	setVideoFile( set->value("filename", videoFile()).toString() );

	set->endGroup();
}

#include "moc_videopreview.cpp"

