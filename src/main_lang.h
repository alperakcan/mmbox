/*
    Copyright (C) 2002  Alper Akcan

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    distch@hotmail.com
    icq : 651561
    tel : 0532 414 05 12
*/
#ifdef LANG_TR

/* turkish u big,	ascii 220	"�"
 * turkish u small,	ascii 252 	"�"
 * turkish o big,	ascii 214	"�"
 * turkish o small,	ascii 246	"�"
 * turkish c big,	ascii 199	"�"
 * turkish c small,	ascii 231	"�"
 * turkish s big,	ascii 222	"�"
 * turkish s small,	ascii 254	"�"
 * turkish i big,	ascii 221	"�"
 * turkish i small,	ascii 253	"�"
 */

#define TEXT_OK "Tamam"
#define TEXT_CANCEL "�ptal"
#define TEXT_ERR "Hata Olu�tu !"
#define TEXT_QUEST "Emin misiniz ?"
// main.c
#define TEXT_QUITALL "Tetromino Box`� kapatmak istediginizden emin misiniz?"
// ir.c
#define IR_CODE_TEXTS "��k��", "Oynat", "Beklet", "Durdur", "Sonraki", "�nceki", "Rastgele", "Tekrar", "JBOX", "JB(MP3)", "JB(CD)", "SES +", "SES -"
#define IRMAN_CODES_TEXT "IRMAN Kodlar� Kurulumu"
#define IRMAN_HINT "Degi�tirmek istediginiz kodun butonuna bas�n, ��kmak i�in tamama bas�n."
#define IRMAN_HINT2 "Fonksiyona atamak istediginiz tu�a bas�n (Zamana��m� be� saniyedir)."
#define IRMAN_NO_CODE "Gecerli Bir Kod Al�namad�."
// clock.c
#define MMDDYYYY "aa : gg : yyyy"
#define HHMMSS "ss : dd : ss"
#define CLOCK_CAPTION "Saat Ayarlar�."
// dirbrowser.c
#define TEXT_ADDFILE "+Dosya"
#define TEXT_ADDFILEALL "+Dosyalar"
#define TEXT_MATCH "Tip :"
#define TEXT_ADDDIR "+ Dizin"
#define TEXT_DBSTATUS "Dizin: %d, Dosya: %d (Tip: %s)"
#define TEXT_DBTOP "Dizin Gezgini"
// jbox
#define TEXT_SHUFFLE "(S)Rastgele"
#define TEXT_REPEAT "(R)Tekrar"
// jbox/mp3.c
#define TEXT_ID3v1	"�arki �smi\n\n"\
			"Artist\n\n"\
			"Alb�m\n\n"\
			"Y�l\n\n"\
			"Not\n\n"\
			"No\n\n"\
			"Stil\n\n"
#define TEXT_NOFILE "Dosya Bulunamad� ?!"
#define TEXT_NOLAYER "Bu bir Layer I,II,III dosyas� degil ?!"
#define TEXT_ID3v1TOP "%s, ID3.v1 & Bilgi"
// jbox/playlist.c
#define TEXT_DELLALL "Playlistten t�m dosyalar� kald�rmak istiyor musunuz ?"
#define TEXT_PLAY "Oynat"
#define TEXT_ID "ID3.v1 & Bilgi"
#define TEXT_REMOVE "Kald�r"
#define TEXT_FUNC "Fonksiyonlar"
#define TEXT_NOAUDIOCD "Bu bir audio cd degil ??"
#define TEXT_CDDB "CDDB Bilgisi"
#define TEXT_CDDEVICE "CD S�r�c�s�"
// jbox/updater.c
#define TEXT_ELAPSED "Ge�en S�re :"
#define TEXT_REMAINING "Kalan S�re :"
// jbox/games
#define LEGEND "Anahtar :"
#define PRESS_START "!! BA�LAT !!"
#define GAME_OVER "!! OYUN B�TT� !!"
#define PAUSE "!! BEKLEMEDE !!"
#define TETRIS_LEGEND	"Sol : Number Pad 4\n"\
			"Sag : Number Pad 6\n"\
			"Beklet / Devam : p\n"\
			"Ba�lat / Tekrar : r\n"\
			"Kapat : q\n"\
			"Simge Durumu : m\n"
#define TETRIS_HINT "* Numlock ON"
#define MINES_LEGEND	"Kutuyu a�mak :\n"\
			"       sol mouse butonu\n"\
			"Bomba / Soru i�areti :\n"\
			"       sag mouse butonu\n"\
			"Tekrar : r\n"\
			"Kapat : q\n"\
			"Simge Durumu : m\n"
#define MINES_HINT "* Bo� kutunun etraf�nda bomba olamaz ;)"

#else
// LANG_EN

#define TEXT_OK "OK"
#define TEXT_CANCEL "Cancel"
#define TEXT_ERR "Error Occured !"
#define TEXT_QUEST "Are You Sure ?"
// main.c
#define TEXT_QUITALL "Are you sure you want to close Tetromino Box ?"
// ir.c
#define IR_CODE_TEXTS "QUIT", "Play", "Pause", "Stop", "Next", "Prev", "Shuffle", "Repeat", "JBOX", "JB(MP3)", "JB(CD)", "VOL +", "VOL -"
#define IRMAN_CODES_TEXT "IRMAN Codes Settings"
#define IRMAN_HINT "Press the button of the code that you want to change, or ok to close this."
#define IRMAN_HINT2 "Press button.. (Timeout = 5 secs.)"
#define IRMAN_NO_CODE "Couldnt Get a Valid Code."
// clock.c
#define MMDDYYYY "mm : dd : yyyy"
#define HHMMSS "hh : mm : ss"
#define CLOCK_CAPTION "Clock Settings."
// dirbrowser.c
#define TEXT_ADDFILE "+ File"
#define TEXT_ADDFILEALL "+ File All"
#define TEXT_MATCH "Match :"
#define TEXT_ADDDIR "+ Dir"
#define TEXT_DBSTATUS "Dirs: %d, Files: %d (Match: %s)"
#define TEXT_DBTOP "Directory Browser"
// jbox
#define TEXT_SHUFFLE "(S)huffle"
#define TEXT_REPEAT "(R)epeat"
// jbox/mp3.c
#define TEXT_ID3v1	"Song Name\n\n"\
			"Artist\n\n"\
			"Album\n\n"\
			"Year\n\n"\
			"Note\n\n"\
			"Track\n\n"\
			"Style\n\n"
#define TEXT_NOFILE "No Such File ?!"
#define TEXT_NOLAYER "This is not a Layer I,II,III file ?!"
#define TEXT_ID3v1TOP "%s, ID3.v1 & Info"
// jbox/playlist.c
#define TEXT_DELLALL "This will delete all files from your playlist !!"
#define TEXT_PLAY "PLAY"
#define TEXT_ID "ID3.v1 & Info"
#define TEXT_REMOVE "Remove"
#define TEXT_FUNC "Functions"
#define TEXT_NOAUDIOCD "NOT an Audio CD ??"
#define TEXT_CDDB "CDDB Info"
#define TEXT_CDDEVICE "CD Device"
// jbox/updater.c
#define TEXT_ELAPSED "Elapsed :"
#define TEXT_REMAINING "Remaining :"
// jbox/games
#define LEGEND "Legend :"
#define PRESS_START "!! PRESS START !!"
#define GAME_OVER "!! GAME OVER !!"
#define PAUSE "!! PAUSED !!"
#define TETRIS_LEGEND	"Left : Number Pad 4\n"\
			"Right : Number Pad 6\n"\
			"Pause / Continue : p\n"\
			"Start / Restart : r\n"\
			"Close : q\n"\
			"Minimize : m\n"
#define TETRIS_HINT "* Turn Numlock ON"
#define MINES_LEGEND	"Open Box :\n"\
			"       Left mouse button\n"\
			"Bomb / Question Mark :\n"\
			"       Right mouse button\n"\
			"Restart : r\n"\
			"Close : q\n"\
			"Minimize : m\n"
#define MINES_HINT "   * Sides of empty boxes are safe ;)   "
#endif
