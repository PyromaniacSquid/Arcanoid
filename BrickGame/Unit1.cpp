//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
#pragma resource ("*.Surface.fmx", _PLAT_MSWINDOWS)
#pragma resource ("*.Windows.fmx", _PLAT_MSWINDOWS)

TForm1 *Form1;
bool PreGame = true;
bool OutOfZoneX = false;
bool OutOfZoneY = false;
int Iters = 0;
int Stage = 0;
int Speed = 2;
Vect Direction;
map<int, Brick> Bricks;
TRectangle* rects[20];
int Destroyed = 0;
unsigned int colors[20] = {0xFFFFA500, 0xFFFF1493, 0xFF228B22, 0xFFFFD700, 0xFF4B0082,
			  0xFFDDA0DD, 0xFFF5DEB3, 0xFF8B008B, 0xFF0000FF,  0xFF3CB371,
			  0xFFFFF8DC, 0xFF5F9EA0, 0xFFA52A2A, 0xFFF5F5DC, 0xFF00FFFF,
			  0xFF4B0082, 0xFFFFFACD, 0xFFADFF2F, 0xFFFF1493, 0xFF228B22};
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	MediaPlayer1->Play();
	MediaPlayer1->Volume = 0.2;
	const char* rectsNamePrefix = "Bricka";
	for (int i=0;i < 20 ;i++)
	{
		rects[i] = new TRectangle(this);
		rects[i]->Name = rectsNamePrefix + IntToStr(i+1);
		rects[i]->Position->X = (i % 5) * 100;
		rects[i]->Position->Y = 33 * (i / 5);
		rects[i]->Width = 100;
		rects[i]->Height = 33;
		rects[i]->Fill->Kind = TBrushKind::Solid;
		rects[i]->Fill->Color = colors[i];
		rects[i]->Parent = this;
		Bricks[i] = Brick(rects[i]->Position->X,
							rects[i]->Position->X + rects[i]->Width,
							rects[i]->Position->Y + rects[i]->Height/2);
	}
	BrickRects = {Brick1, Brick2, Brick3, Brick4, Brick5,
								Brick6, Brick7, Brick8, Brick9, Brick10,
								Brick11, Brick12,Brick13,Brick14,Brick15,
								Brick16,Brick17,Brick18,Brick19,Brick20};
	Form1->Rectangle1->Position->X = 175;
	Form1->Circle1->Position->X = 230;
	Label5->Text = Direction.X;
	Label6->Text = Direction.Y;

}
//---------------------------------------------------------------------------
void CheckForBrick(float x, float y){
	int sideMod = 0;
	if (((int)x % 100 == 0) && ((int)y % 33 != 0)) {
		 sideMod = Direction.X > 0 ? 1 : -1;
	}
	int col = trunc(x / 100);
	int row = ceil(y / 33);
	int brickIndex = (row-1)*5 + col + sideMod * (col % 4 != 0);
	Form1->Label7->Text = IntToStr(col) + " " + IntToStr(row) + " "  + IntToStr(brickIndex);
	if ((brickIndex >= 0) && (brickIndex < 20)) {
		if (rects[brickIndex] != NULL)
		{
			auto Target = (Form1->FindComponent(Form1->Label7->Text));
			rects[brickIndex]->DisposeOf();
			rects[brickIndex]=NULL;
			Destroyed++;
			bool sideHit = sideMod != 0;
			OutOfZoneX = false;
			OutOfZoneY = false;
			Direction.ChangeDir(sideHit);
		}
	}
}

void __fastcall TForm1::MoveAnimProcess(TObject *Sender)
{
	if (!PreGame) {

		float newX = ScreenToClient(Screen->MousePos()).x - 75;

		Label1->Text = FloatToStr(newX);
        Label4->Text = Rectangle1->Position->Y;
		if ((newX > 0) && (newX <= 350)) {
			Rectangle1->Position->X = newX;
		}
		else{
			Rectangle1->Position->X = newX > 0 ? 350 : 0;
		}
		MoveAnim->StopValue = Rectangle1->Position->X;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TForm1::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  float X, float Y)
{
	if (Destroyed < 20) {
	CIrcleMoveX->Enabled = true;
	CircleMoveY->Enabled = true;
	PreGame = !PreGame;
	MoveAnim->Enabled = true;
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::CIrcleMoveXProcess(TObject *Sender)
{
	CIrcleMoveX->StartValue = Circle1->Position->X;
	CIrcleMoveX->StopValue += Direction.X * Speed;
	Label2->Text = Circle1->Position->X;

	if (
		((Circle1->Position->X <= 0) && (Direction.X < 0))
		||
		((Circle1->Position->X >= 465) && (Direction.X > 0))
		)
	{
		if (!OutOfZoneX) {
			OutOfZoneX = true;
			Direction.ChangeDir(true);
		}
		if (abs(Direction.X) < 2) {
			Direction.X *= 1.125;
		}
		Label5->Text = Direction.X;
		Label6->Text = Direction.Y;
	}
	else {
       OutOfZoneX = false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CircleMoveYProcess(TObject *Sender)
{
	CircleMoveY->StartValue = Circle1->Position->Y;
	CircleMoveY->StopValue += Direction.Y * Speed;
	Label3->Text = Circle1->Position->Y;
	if ((Rectangle1->Position->Y - Rectangle1->Height - Circle1->Position->Y < 5) && (Direction.Y > 0) )
		{
			if ((Circle1->Position->X >= Rectangle1->Position->X - 35) &&
				(Circle1->Position->X <= Rectangle1->Position->X + Rectangle1->Width+35)) {
				float mid = Rectangle1->Position->X + (Rectangle1->Width / 2);
				if (Direction.X > 0) {
				   float mod = Circle1->Position->X > mid ? 1.5 : 0.5;
				   Direction.Y *= mod;
				}
				else{
					float mod = Circle1->Position->X < mid ? 1.5 : 0.5;
					Direction.Y *= mod;
				}
				if (abs(Direction.Y) < 1) {
					Direction.Y = Direction.Y > 0 ? 1 : -1;
				}
				if (!OutOfZoneY) {
					OutOfZoneY = true;
					Direction.ChangeDir(false);
				}
				Label5->Text = Direction.X;
				Label6->Text = Direction.Y;
			}
			else {
				CIrcleMoveX->Enabled = false;
				CircleMoveY->Enabled = false;
				PreGame = !PreGame;
				MoveAnim->Enabled = false;
				MoveAnim->StartValue=175;;
				MoveAnim->StopValue=175;
				Form1->Rectangle1->Position->X = 175;
				Form1->Circle1->Position->X = 230;
				CIrcleMoveX->StartValue = 230;
				CIrcleMoveX->StopValue = 230;
				Form1->Circle1->Position->Y = 648;
				CircleMoveY->StartValue = 648;
				CircleMoveY->StopValue = 648;
				Direction.X = 1;
				Direction.Y = -1;
            }
	}
	else if ((Circle1->Position->Y <= 128)) {
			//OutOfZoneX = Circle1->Position->Y > 0 || OutOfZoneX;
			if (Circle1->Position->Y <= 0 && !OutOfZoneY)
			{
				OutOfZoneY = true;
				Direction.ChangeDir(false);
			}
			else {
			//CheckForBrick(Circle1->Position->X, Circle1->Position->Y);
			CheckForBrick(CIrcleMoveX->StopValue, CircleMoveY->StopValue);
			}
		 }
	else{
		OutOfZoneY = false;
	}
	if (Destroyed == 20) {
		PreGame=true;
		CIrcleMoveX->Enabled = false;
		CircleMoveY->Enabled = false;
		MoveAnim->Enabled = false;
		ShowMessage("You won!");
	}
	//
}

void __fastcall TForm1::FormGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
          bool &Handled)
{
	float DX, DY;

	if (EventInfo.GestureID == igiPan) {
		if (EventInfo.Flags.Contains(TInteractiveGestureFlag::gfBegin)
		  && (Sender == ToolbarPopup || EventInfo.Location.Y > (ClientHeight - 70))) {
			FGestureOrigin = EventInfo.Location;
			FGestureInProgress = true;
		}
	}

	if (FGestureInProgress && EventInfo.Flags.Contains(TInteractiveGestureFlag::gfEnd)) {
		FGestureInProgress = false;
		DX = EventInfo.Location.X - FGestureOrigin.X;
		DY = EventInfo.Location.Y - FGestureOrigin.Y;
		if (fabs(DY) > fabs(DX)) {
			ShowToolbar(DY < 0);
		}
	}
}
//---------------------------------------------------------------------------
void TForm1::ShowToolbar(bool AShow)
{
  ToolbarPopup->Width = ClientWidth;
  ToolbarPopup->PlacementRectangle->Rect = TRect(0, ClientHeight-ToolbarPopup->Height, ClientWidth-1, ClientHeight-1);
  ToolbarPopupAnimation->StartValue = ToolbarPopup->Height;
  ToolbarPopupAnimation->StopValue = 0;

  ToolbarPopup->IsOpen = AShow;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar,
          TShiftState Shift)
{
  if (Key == vkEscape) {
	  ShowToolbar(!ToolbarPopup->IsOpen);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolbarCloseButtonClick(TObject *Sender)
{
	Application->Terminate();
}
//---------------------------------------------------------------------------
