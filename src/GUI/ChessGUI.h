#include <wx/wxprec.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/infobar.h>
#include <wx/appprogress.h>
#include <wx/bitmap.h>
#include <string>
#include <fstream>
#include "../chess/ClassicBitBoard.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
class ChessGUI: public wxApp
{
public:
    bool OnInit() override;

    virtual ~ChessGUI(){};

private:
};
wxDECLARE_APP(ChessGUI);

//https://stackoverflow.com/questions/61803924/sample-code-for-c-wxwidgets-blit-an-image-to-the-canvas
class ChessBoardPanel: public wxPanel{
public:
    ChessBoardPanel(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style,
                    const wxString &name);

public:
    void OnPaint(wxPaintEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnIdle(wxIdleEvent& event);
    void DrawBoard(wxPaintDC& dc);
    //Square, a8 = 0, h1 = 63;
    void move(int from, int to);
private:
    void loadBitmaps(std::string bitmaps);
    wxBitmap m_blackRook;
    wxBitmap m_blackKnight;
    wxBitmap m_blackBishop;
    wxBitmap m_blackQueen;
    wxBitmap m_blackKing;
    wxBitmap m_blackPawn;

    wxBitmap m_whiteRook;
    wxBitmap m_whiteKnight;
    wxBitmap m_whiteBishop;
    wxBitmap m_whiteQueen;
    wxBitmap m_whiteKing;
    wxBitmap m_whitePawn;

    //Square, a8 = 0, h1 = 63;
    const wxPoint squares[64]{
        wxPoint(700,700),wxPoint(600,700),wxPoint(500,700),wxPoint(400,700),wxPoint(300,700),wxPoint(200,700),wxPoint(100,700),wxPoint(0,700),
        wxPoint(700,600),wxPoint(600,600),wxPoint(500,600),wxPoint(400,600),wxPoint(300,600),wxPoint(200,600),wxPoint(100,600),wxPoint(0,600),
        wxPoint(700,500),wxPoint(600,500),wxPoint(500,500),wxPoint(400,500),wxPoint(300,500),wxPoint(200,500),wxPoint(100,500),wxPoint(0,500),
        wxPoint(700,400),wxPoint(600,400),wxPoint(500,400),wxPoint(400,400),wxPoint(300,400),wxPoint(200,400),wxPoint(100,400),wxPoint(0,400),
        wxPoint(700,300),wxPoint(600,300),wxPoint(500,300),wxPoint(400,300),wxPoint(300,300),wxPoint(200,300),wxPoint(100,300),wxPoint(0,300),
        wxPoint(700,200),wxPoint(600,200),wxPoint(500,200),wxPoint(400,200),wxPoint(300,200),wxPoint(200,200),wxPoint(100,200),wxPoint(0,200),
        wxPoint(700,100),wxPoint(600,100),wxPoint(500,100),wxPoint(400,100),wxPoint(300,100),wxPoint(200,100),wxPoint(100,100),wxPoint(0,100),
        wxPoint(700,000),wxPoint(600,000),wxPoint(500,000),wxPoint(400,000),wxPoint(300,000),wxPoint(200,000),wxPoint(100,000),wxPoint(0,000)
    };
    //chess::ClassicBitBoard internalBoard = chess::ClassicBitBoard();

};
class ChessClock: public wxPanel{
public:
    ChessClock(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style,
               const wxString &name);

    void OnResize(wxSizeEvent& event);
    //void OnIdle(wxIdleEvent& event);

private:
    wxFont* font;
    wxStaticText* whitePlayer;
    wxStaticText* whiteClock;
    wxStaticText* blackPlayer;
    wxStaticText* blackClock;
    wxBoxSizer* clock_sizer;
    wxBoxSizer* top_sizer;
};
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    virtual ~MyFrame(){};
private:
    wxGauge* progressBar =nullptr;
    wxInfoBar* infoBar = nullptr;
    //void OnHello(wxCommandEvent& event);
    //void OnExit(wxCommandEvent& event);
    //void OnAbout(wxCommandEvent& event);
    //void OnSaveAs(wxCommandEvent& event);

    ChessBoardPanel* m_board;


    wxAuiManager m_mgr = wxAuiManager(this,wxAUI_MGR_LIVE_RESIZE |
                                           wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_VENETIAN_BLINDS_HINT |
                                           wxAUI_MGR_TRANSPARENT_HINT |wxAUI_MGR_HINT_FADE |wxAUI_MGR_ALLOW_ACTIVE_PANE );
wxDECLARE_EVENT_TABLE();
};


enum
{
    ID_NewProject,ID_OpenProject,ID_CloseProject
};
