extern BOOL DialogMsgProcDec(HWND hWndDlg, UINT Message, WPARAM wParam, LPARAM lParam);
extern int ShowDlg4RawAAC();

static const char* mpeg4AudioNames[]=
{
	"Raw PCM",
	"AAC Main",
	"AAC LC (Low Complexity)",
	"AAC SSR",
	"AAC LTP (Long Term Prediction)",
	"AAC HE (High Efficiency)",
	"AAC Scalable",
	"TwinVQ",
	"CELP",
	"HVXC",
	"Reserved",
	"Reserved",
	"TTSI",
	"Main synthetic",
	"Wavetable synthesis",
	"General MIDI",
	"Algorithmic Synthesis and Audio FX",
// defined in MPEG-4 version 2
	"ER AAC LC (Low Complexity)",
	"Reserved",
	"ER AAC LTP (Long Term Prediction)",
	"ER AAC Scalable",
	"ER TwinVQ",
	"ER BSAC",
	"ER AAC LD (Low Delay)",
	"ER CELP",
	"ER HVXC",
	"ER HILN",
	"ER Parametric",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};
