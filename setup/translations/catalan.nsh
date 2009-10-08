﻿;;
;;  catalan.nsh
;;
;;  Catalan language strings for the Windows SMPlayer NSIS installer.
;;  Save file as UTF-8
;;

; Startup
LangString SMPLAYER_INSTALLER_IS_RUNNING ${LANG_CATALAN} "The installer is already running."
LangString SMPLAYER_INSTALLER_PREV_ALL_USERS ${LANG_CATALAN} "SMPlayer has been previously installed for all users.$\nPlease restart the installer with Administrator privileges."
LangString SMPLAYER_INSTALLER_PREV_VERSION ${LANG_CATALAN} "SMPlayer has already been installed.$\nDo you want to remove the previous version before installing $(^Name)?"

; Components Page
LangString MPLAYER_CODEC_INFORMATION ${LANG_CATALAN} "The binary codec packages add support for codecs that are not yet implemented natively, like newer RealVideo variants and a lot of uncommon formats.$\nNote that they are not necessary to play most common formats like DVDs, MPEG-1/2/4, etc."

; MPlayer Section
LangString MPLAYER_IS_INSTALLED ${LANG_CATALAN} "MPlayer is already installed. Re-Download?"
LangString MPLAYER_IS_DOWNLOADING ${LANG_CATALAN} "Downloading MPlayer..."
LangString MPLAYER_DL_FAILED ${LANG_CATALAN} "Failed to download MPlayer:"
LangString MPLAYER_INST_FAILED ${LANG_CATALAN} "Failed to install MPlayer. MPlayer is required for playback."

; Codecs Section
LangString CODECS_IS_INSTALLED ${LANG_CATALAN} "MPlayer codecs are already installed. Re-Download?"
LangString CODECS_IS_DOWNLOADING ${LANG_CATALAN} "Downloading MPlayer codecs..."
LangString CODECS_DL_FAILED ${LANG_CATALAN} "Failed to download MPlayer codecs:"
LangString CODECS_INST_FAILED ${LANG_CATALAN} "Failed to install MPlayer codecs."

; Version information
LangString VERINFO_IS_DOWNLOADING ${LANG_CATALAN} "Downloading version information..."
LangString VERINFO_DL_FAILED ${LANG_CATALAN} "Failed to download version info:"
LangString VERINFO_IS_MISSING ${LANG_CATALAN} "Version file missing version information. Setup will use a default version."

; Uninstaller
LangString UNINSTALL_ABORTED ${LANG_CATALAN} "Uninstall aborted by user."
LangString UNINSTALL_INSTALLED_ALL_USERS ${LANG_CATALAN} "SMPlayer has been installed for all users.$\nPlease restart the uninstaller with Administrator privileges to remove it."
LangString SMPLAYER_NOT_INSTALLED ${LANG_CATALAN} "It does not appear that SMPlayer is installed in the directory '$INSTDIR'.$\r$\nContinue anyway (not recommended)?"

; Vista & Later Default Programs Registration
LangString APPLICATION_DESCRIPTION ${LANG_CATALAN} "SMPlayer is a complete front-end for MPlayer, from basic features like playing videos, DVDs, VCDs to more advanced features like support for MPlayer filters, edl lists, and more."