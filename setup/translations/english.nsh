;;
;;  english.nsh
;;
;;  English language strings for the Windows SMPlayer NSIS installer.
;;  Save file with ANSI encoding
;;
;;  Author: redxii
;;

; Components Page
LangString MPLAYER_CODEC_INFORMATION ${LANG_ENGLISH} "The binary codec packages add support for codecs that are not yet implemented natively, like newer RealVideo variants and a lot of uncommon formats.$\nNote that they are not necessary to play most common formats like DVDs, MPEG-1/2/4, etc."

; MPlayer Section
LangString MPLAYER_IS_INSTALLED ${LANG_ENGLISH} "MPlayer is already installed. Re-Download?"
LangString MPLAYER_IS_DOWNLOADING ${LANG_ENGLISH} "Downloading MPlayer..."
LangString MPLAYER_DL_FAILED ${LANG_ENGLISH} "Failed to download MPlayer:"
LangString MPLAYER_INST_FAILED ${LANG_ENGLISH} "Failed to install MPlayer. MPlayer is required for playback."

; Codecs Section
LangString CODECS_IS_INSTALLED ${LANG_ENGLISH} "MPlayer codecs are already installed. Re-Download?"
LangString CODECS_IS_DOWNLOADING ${LANG_ENGLISH} "Downloading MPlayer codecs..."
LangString CODECS_DL_FAILED ${LANG_ENGLISH} "Failed to download MPlayer codecs:"
LangString CODECS_INST_FAILED ${LANG_ENGLISH} "Failed to install MPlayer codecs."

; Version information
LangString VERINFO_IS_DOWNLOADING ${LANG_ENGLISH} "Downloading version information..."
LangString VERINFO_DL_FAILED ${LANG_ENGLISH} "Failed to download version info:"
LangString VERINFO_IS_MISSING ${LANG_ENGLISH} "Version file missing version information. Setup will use a default version."