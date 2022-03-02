#include "ChessGUI.h"
// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
class MyMenuBar: public wxMenuBar{
public:
    MyMenuBar(){
        init();
    }
private:
    void init();
private:
    wxMenu* menuFile = nullptr;
};


void MyMenuBar::init() {
    menuFile = new wxMenu();
    menuFile->Append(ID_NewProject, "New Project");
    menuFile->Append(ID_OpenProject, "Open Project");
    menuFile->Append(ID_CloseProject, "Close Project");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_SAVE);
    menuFile->Append(wxID_SAVEAS);
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    this->Append(menuFile, "File");
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
                //EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
                //EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
                //EVT_MENU(wxID_SAVEAS, MyFrame::OnSaveAs)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(ChessGUI);
bool ChessGUI::OnInit() {
    ::wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame("Hello World", wxPoint(50, 50), wxSize(900, 680));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        :wxFrame(NULL, wxID_ANY, title, pos, size)
{

    this->m_board = new ChessBoardPanel(this,wxID_ANY, wxDefaultPosition,wxSize(800,800),wxTAB_TRAVERSAL | wxNO_BORDER,"Board");
    // notify wxAUI which frame to use
    m_mgr.AlwaysUsesLiveResize();
    m_mgr.SetManagedWindow(this);
    // create several text controls

    auto t = new wxAuiNotebook(this);
    m_mgr.AddPane(t,wxCenter,wxT("Tabs"));
    m_mgr.AddPane(this->m_board,wxCenter, wxT("Board"));
    m_mgr.GetPane(this->m_board).MaximizeButton();
    m_mgr.GetPane(this->m_board).MinimizeButton();
    m_mgr.AddPane(new ChessClock(
            this,wxID_ANY, wxDefaultPosition,wxSize(800,800),wxTAB_TRAVERSAL | wxNO_BORDER,"Clock"
            ),wxCenter,wxT("Clock"));



    SetMenuBar( new MyMenuBar );

    // tell the manager to "commit" all the changes just made
    m_mgr.Update();


}

void ChessBoardPanel::loadBitmaps(std::string bitmaps) {
// Load the image.
    wxImage im(bitmaps, wxBITMAP_TYPE_PNG);
    std::ifstream file(bitmaps);
    if (file.fail()){
        std::cout << "fail"<< std::endl;
    }

    // Extract the images of the pieces from the larger image.
    wxBitmap b(im);
    m_blackKing = b.GetSubBitmap(wxRect(5,113,100,100));
    m_blackQueen = b.GetSubBitmap(wxRect(110,113,100,100));
    m_blackBishop = b.GetSubBitmap(wxRect(215,113,100,100));
    m_blackKnight = b.GetSubBitmap(wxRect(323,113,100,100));
    m_blackRook = b.GetSubBitmap(wxRect(433,113,100,100));
    m_blackPawn = b.GetSubBitmap(wxRect(535,113,100,100));

    m_whiteKing = b.GetSubBitmap(wxRect(5,13,100,100));
    m_whiteQueen = b.GetSubBitmap(wxRect(110,13,100,100));
    m_whiteBishop = b.GetSubBitmap(wxRect(215,13,100,100));
    m_whiteKnight = b.GetSubBitmap(wxRect(323,13,100,100));
    m_whiteRook = b.GetSubBitmap(wxRect(433,13,100,100));
    m_whitePawn = b.GetSubBitmap(wxRect(535,13,100,100));
}

ChessBoardPanel::ChessBoardPanel(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style,
                                 const wxString &name) : wxPanel(parent, winid, pos, size, style, name) {
    //load data
    this->loadBitmaps("board_pieces.png");

    //Double buffering
    this->SetDoubleBuffered(true);

    //set events
    this->Bind(wxEVT_PAINT, &ChessBoardPanel::OnPaint, this);
    this->Bind(wxEVT_IDLE, &ChessBoardPanel::OnIdle, this);
    parent->Bind(wxEVT_SIZE, &ChessBoardPanel::OnResize,this);

}

void ChessBoardPanel::OnPaint(wxPaintEvent &event) {
    int width, height;
    this->GetSize(&width,&height);
    double scale = (double)std::min(width,height)/800;
    this->SetSize(std::min(width,height),std::min(width,height));

    wxPaintDC dc(this);
    dc.Clear();
    dc.SetUserScale(scale,scale);

    // Draw the light squares
    dc.SetPen(wxColour(209,139,71));
    dc.SetBrush(wxColour(209,139,71));
    dc.DrawRectangle(0,0,800,800);

    // Draw the dark squares
    dc.SetPen(wxColour(255,206,158));
    dc.SetBrush(wxColour(255,206,158));
    for ( int i = 0 ; i< 8 ; ++i )
    {
        for ( int j = i%2 ; j< 8 ; j+=2 )
        {
            dc.DrawRectangle(i*100,j*100,100,100);
        }
    }
    this->DrawBoard(dc);

    event.Skip();

}

void ChessBoardPanel::DrawBoard(wxPaintDC& dc) {
    // Draw the black pieces
    dc.DrawBitmap(m_blackRook, 0, 0, true);
    dc.DrawBitmap(m_blackKnight, 100, 0, true);
    dc.DrawBitmap(m_blackBishop, 200, 0, true);
    dc.DrawBitmap(m_blackQueen, 300, 0, true);
    dc.DrawBitmap(m_blackKing, 400, 0, true);
    dc.DrawBitmap(m_blackBishop, 500, 0, true);
    dc.DrawBitmap(m_blackKnight, 600, 0, true);
    dc.DrawBitmap(m_blackRook, 700, 0, true);

    for ( int i = 0 ; i < 8 ; ++i )
    {
        dc.DrawBitmap(m_blackPawn, 100*i, 100, true);
    }

    // Draw the white pieces
    dc.DrawBitmap(m_whiteRook, 0, 700, true);
    dc.DrawBitmap(m_whiteKnight, 100, 700, true);
    dc.DrawBitmap(m_whiteBishop, 200, 700, true);
    dc.DrawBitmap(m_whiteQueen, 300, 700, true);
    dc.DrawBitmap(m_whiteKing, 400, 700, true);
    dc.DrawBitmap(m_whiteBishop, 500, 700, true);
    dc.DrawBitmap(m_whiteKnight, 600, 700, true);
    dc.DrawBitmap(m_whiteRook, 700, 700, true);

    for ( int i = 0 ; i < 8 ; ++i )
    {
        dc.DrawBitmap(m_whitePawn, 100*i, 600, true);
    }

}

void ChessBoardPanel::OnResize(wxSizeEvent &event) {
    this->Refresh();
}
void ChessBoardPanel::OnIdle(wxIdleEvent &event) {
    this->Refresh();
}


ChessClock::ChessClock(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style,
                       const wxString &name) : wxPanel(parent, winid, pos, size, style, name) {
    int clockHeight = (size.GetHeight()-40)/4;
    this->font = new wxFont();
    this->font->SetPointSize((int)(clockHeight/1.333));
    this->font->SetFamily(wxFONTFAMILY_TELETYPE);
    this->font->SetWeight(wxFONTWEIGHT_BOLD);

    //init clocks

    this->whiteClock = new wxStaticText(this,wxID_ANY,"0:00:00 [W]",wxPoint(0,0),wxSize(100,40),wxALIGN_CENTRE_HORIZONTAL);
    this->blackClock = new wxStaticText(this,wxID_ANY,"0:00:00 [B]",wxPoint(0,40),wxSize(100,40),wxALIGN_CENTRE_HORIZONTAL);
    this->whiteClock->SetBackgroundColour(wxColour(255,255,255));
    this->whiteClock->SetForegroundColour(wxColour(0,0,0));
    this->blackClock->SetBackgroundColour(wxColour(0,0,0));
    this->blackClock->SetForegroundColour(wxColour(255,255,255));
    this->whiteClock->SetFont(*font);
    this->blackClock->SetFont(*font);

    //init playernames
    this->whitePlayer = new wxStaticText(this,wxID_ANY,"name1",wxPoint(0,0),wxSize(100,20));
    this->blackPlayer = new wxStaticText(this,wxID_ANY,"name2",wxPoint(0,0),wxSize(100,20));
    this->whitePlayer->SetBackgroundColour(wxColour(0,255,255));
    this->whitePlayer->SetForegroundColour(wxColour(0,0,0));
    this->blackPlayer->SetBackgroundColour(wxColour(50,0,0));
    this->blackPlayer->SetForegroundColour(wxColour(255,255,255));

    //clock sizer
    this->clock_sizer = new wxBoxSizer(wxVERTICAL );
    this->clock_sizer->Add(this->whiteClock, 1, wxEXPAND,0);
    this->clock_sizer->Add(this->blackClock, 1, wxEXPAND,0);

    //top sizer
    this->top_sizer = new wxBoxSizer(wxVERTICAL);
    this->top_sizer->Add(this->whitePlayer,1,wxEXPAND,0);
    this->top_sizer->Add(clock_sizer,4,wxEXPAND,0);
    this->top_sizer->Add(this->blackPlayer,1,wxEXPAND,0);

    SetSizerAndFit(this->top_sizer); // use the sizer for layout and size window

    this->Bind(wxEVT_SIZE,&ChessClock::OnResize,this);
}

void ChessClock::OnResize(wxSizeEvent& event) {
    this->font->SetPointSize((int)((event.GetSize().GetHeight()-40)/4));
    this->whiteClock->SetFont(*font);
    this->blackClock->SetFont(*font);
    event.Skip();
}
