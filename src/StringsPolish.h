// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

// These are text strings used in the game

#define LEVEL_0 L"<Początek Przekazu>\n\nW imieniu Imperatora zostałeś zintegrowany z Imperialnym Myśliwcem XZ-75fh.\n\nXZ-75fh jest bardzo łatwy w opanowaniu, dysponuje systemem podtrzymywania życia, posiada różnorodne uzbrojenie i dodatkowe Zdolności.\n\n Gdy zaczniesz spełniać wolę Imperatora, otrzymasz dostęp do dodatkowych funkcji statku oraz wydłużonego życia. \n\n\n\nNasze pokojowe osady w sektorze omeka zostały zmiecione z powierzchni ziemi, Twoim zadaniem jest więc rozbicie sił przeciwnika.\n\nWolą Imperatora jest to, abyś zwyciężył siły heretyków!\n\n\n\nOczyść swój umysł i ciało!\n\n<Koniec Przekazu>"
#define LEVEL_1	L"<Początek Przekazu>\n\nUdowodniłeś, że jesteś godzien służby w siłach Imperatora oraz że jesteś świetnym pilotem. \n\nWywiad wykrył koncentrację sił przeciwnika w tym sektorze. Są one zagrożeniem dla Imperialnych linii zaopatrzeniowych to odległych systemów. \n Zaopatrzenie musi zostać dostarczone do miejsca przeznaczenia albo narazisz się na poważne reperkusje. \n\nHeretycy muszą zostać zniszczeni bez względu na wszystko!\n\nOczyść swój umysł i ciało!!\n\n<Koniec Przekazu>"
#define LEVEL_2	L"<Początek Przekazu>\n\nPod wodzą Imperatora nasze siły ponownie odniosły wspaniałe zwycięstwo. \n\nOdległe systemy są teraz bezpieczne i nasza walka z heretykami może być bezpiecznie kontynuowana.\n\nNiestety, zdrajcy Imperium odkryli słabe punkty w naszym systemie ochrony przed obcymi.\n\nTen sektor jest pod silnym ostrzałem!\n\nMusisz go ochronić!\n\nOczyść swój umysł i ciało!!\n\n<Koniec Przekazu>"
#define LEVEL_3	L"<Początek Przekazu>\n\nPonownie udowodniłeś, jak wiele jesteś warty dla Imperium!\nZ pewnością zostaniesz zapisany na nie starzejących się kartach historii Imperium.\n\nTwoje wysiłki doprowadziły nas na obrzeża nowej kampanii przeciwko obcym heretyckim siłom. \n\nTwoim zadaniem jest odnalezienie i zniszczenie tak wielu heretyków jak to tylko możliwe,\noraz ocena siły wrogich sił w tym sektorze.\n\nOczyść swój umysł i ciało!!\n\n<Koniec Przekazu>"
#define LEVEL_4	L"<Początek Przekazu>\n\nUwaga, Uwaga!\nTo Imperialny przekaz XXZ-4417.\n Obecność obcych sił na wyższym poziomie niż przewidywano, przekazniki radiowe zostały przejete przez zdrajców.\n\nOd tej chwili walczysz już sam! Nie ufaj prze\n\nbzzzzzxxxx\n\nzom! Twoja walka musi być kontyn\n\nxxzzzzzzbbbzzzzhh\n\nana. \n\nOczyść swój umysł\n\nxxxzzzzkkkkkrhhhhhhhhhhzz \n\nPrzekazu>"
#define LEVEL_5	L"gzzzzzfxxx Przekazu>\n\n Heretycy\n\nfrrrrsssssccc\n\n zagrażają planecie matce Imperium.\n Musisz uderzyć w samo serce \nbzzzzzxxxx\nwrogich sił, i musisz to zrobić celnie.\n\nMusisz nadal\nbzzzzzxxxx\n[Dlaczego jesteś przeciwko nam?]\nbzzzzzxxxx\nzwalczać wrogie siły bez pomocy Imperium! \ngzzzfdzzzx\n[Podnieś zasłonę kłamstwa i przejrzyj na oczy]\nkkkrrzzzzzzzh\nOczyść swój umysł, \nxxxzzzzkkkkkrhhhhhhhhhhzz\nnPrzekazu>"
#define LEVEL_6	L"kkkkkzzzzzzzzbh\n\n\nW imieniu Imperatora\n\nUderz pierwszy, uderz mocno i bez litości!\n\nxxxzzzzkkkkkrhhhhhhhhhhzz"
#define LEVEL_7	L"[Musisz przejrzeć na oczy! To nie jest droga dla Ciebie.\n\nTo wszystko kłamstwa!]"
#define LEVEL_8	L"[Zbliżasz się do końca swojej misji.\n\nAle jesteś tu bezcelowo, a my nie chcemy Cię skrzywdzić.]\n\n"
#define LEVEL_9	L"[Jesteś naprawdę groźnym wojownikiem.\nAle Twoja walka nie ma sensu, kontrolujemy zarówno przestrzeń, jak i czas.\n\nJest jeszcze inny sposób.\n\nChcemy odzyskać naszą planetę...\n\nMusimy ją odzyskać. To nie jest atak wrogich sił.]"

#define ABILITY_RAPID_FIRE_HEADLINE 	L"Szybki ostrzał włączony"
#define ABILITY_REGENERATION_HEADLINE	L"Regeneracja włączona"
#define ABILITY_TIME_WARP_HEADLINE		L"Skok w nadprzestrzeń włączony"
#define ABILITY_MISSILES_HEADLINE		L"Rakiety włączone"

// input mappings
// %kf : keyboard fire
// %ke : keyboard use energy
// %k1 : keyboard activate rapid fire
// %k2 : keyboard activate regeneration
// %k3 : keyboard activate time warp
// %k4 : keyboard activate missiles

// %pf : keyboard fire
// %pe : keyboard use energy
// %p1 : keyboard activate rapid fire
// %p2 : keyboard activate regeneration
// %p3 : keyboard activate time warp
// %p4 : keyboard activate missiles

fixme#define ABILITY_RAPID_FIRE 	L"Zdolność Szybkiego ostrzału została włączona.\n\nMożna ją aktywować naciskając klawisze aktywacji energii (prawy przycisk myszy, %ke%pe) oraz klawisz strzelania (lewy przycisk myszy, %kf%pf).\n\n Szybki ostrzał zwiększa ilość wystrzeliwanych pocisków."
fixme#define ABILITY_REGENERATION L"Zdolność Regeneracji została włączona.\n\nMożna ją aktywować naciskając klawisze aktywacji energii (prawy przycisk myszy, %ke%pe).\n\nDostępne zdolności zmienia się kręcąc kółkiem myszy.\n Użyj %k1%p1 aby przełączyć się na Szybki ostrzał oraz %k2%p2 aby włączyć Regenerację.\n\nZdolność Regeneracji odnawia Twoją energię."
fixme#define ABILITY_TIME_WARP L"Zdolność skoku w nadprzestrzeń została włączona.\n\nMożna ją aktywować naciskając klawisze aktywacji energii (prawy przycisk myszy, %ke%pe).\n\nDostępne zdolności zmienia się kręcąc kółkiem myszy.\n Przycisk %k3%p3 może być użyty do szybkiego włączenia Zdolności Skoku w nadprzestrzeń.\n\nSkok w nadprzestrzeń zwalnia poruszanie się przeciwników oraz ich pocisków, tylko XZ-75fh będzie poruszał się z normalną prędkością."
fixme#define ABILITY_MISSILES L"Zdolność Rakiety została włączona.\n\nMożna ją aktywować naciskając klawisze aktywacji energii (prawy przycisk myszy, %ke%pe).\n\nDostępne zdolności zmienia się kręcąc kółkiem myszy.\n Przycisk %k4%p4 może być użyty do szybkiego włączenia Zdolności Rakiety.\n\nRakiety zamieniają przechowaną energię w samonaprowadzające rakiety, które niszczą większośćz widocznych przeciwników."

// Menu strings

// Old version
#define OLD_TEXT L"To jest STARA WERSJA programu.\nPowinieneś pobrać najnowszą wersję dostępna na stronie\n\nwww.spellofplay.com/games/twtpb"
#define OLD_GET L"Pobierz najnowszą wersję"

// Highscores
#define HIGSCORE_HEADLINE L"W pamięci dla"
#define HIGSCORE_PREV L"Poprzedni"
#define HIGSCORE_NEXT L"Następny"

//Main Menu
#define STATUS_NOSERVER L"Nie można skontaktować się z serwerem, sprawdź swoje połączenie z internetem i ustawienia firewalla\n\n"
#define STATUS_DOWNLOADING L"Pobieranie najlepszych wyników...\n\n"
#define STATUS_UPLOADING L"Przesyłanie najlepszego wyniku...\n\n"
#define STATUS_REGISTERED L"Jestes zarejestrowanym Wyznawcą!\n\n"
#define STATUS_COPYRIGHT L"Copyright (C)2007,2008 Spell of Play Studios\n\nwww.spellofplay.com"

#define CONTINUE L"Kontynuuj oczyszczanie"
#define START_GAME L"Rozpocznij oczyszczanie"
fixme #define SHOW_HIGHSCORES L"W pamięci dla Poległych"
#define SHOW_OPTIONS L"Opcje"
#define SHOW_CREDITS L"Kredyty"
#define QUIT_CURRENT_GAME L"Poddaj tę bitwę"
#define EXIT_GAME L"Poddaj się Heretykom"

// Jumpgates
#define JUMPGATES_HEADLINE L"Imperialne bramy nadprzestrzenne"
#define JUMPGATES_HELPTEXT1 L"Wybierz bramę, z której chcesz skorzystać."
#define JUMPGATES_HELPTEXT2 L"Uaktywnij bramy przechodząc kolejne poziomy."
#define JUMPGATES_HELPTEXT2_NOT_REGISTERED L"Uaktywnij bramy przechodząc poszczególne poziomy. \n\nZarejestrowani Wyznawcy mogą zaktywować wszystkie bramy, ale Ty jesteś ograniczony tylko do pierwszych czterech."
#define JUMPGATES_GATE L"Brama %i"

// Legions
#define LEGIO_HEADLINE L"Legiony Imperium"
#define LEGIO_HELPTEXT L"Dołącz do jednego z Legionów Imperium w zależności od Twojego doświadczenia."

#define LEGIO_EASY_HEADLINE L"Legio Hastati"	// not for translation
#define LEGIO_EASY_JOIN L"Dołącz do Legionów Hastati"
#define LEGIO_EASY L"Niedoświadczeni młodzi piloci, którzy dopiero rozpoczęli służbę w siłach imperialnych. \n\nZadaniem tych legionów jest oczyszczanie rejonów mało skażonych przez siły heretyków. Ich służba dla Imperium nie jest w cenie."

#define LEGIO_MEDIUM_HEADLINE L"Legio Principes"	// not for translation
#define LEGIO_MEDIUM_JOIN L"Dołącz do Legionów Principes"
#define LEGIO_MEDIUM L"Rdzeń sił imperialnych, zaprawiony w walce przez liczne wojny. \n\nTe Legiony są wysyłane do silnie zaatakowanych obszarów, w których występuje znaczna obecność obcych. Imperator bardzo docenia przydatność tych sił."

#define LEGIO_HARD_HEADLINE L"Legio Triari"	// not for translation
#define LEGIO_HARD_JOIN L"Dołącz do Legionów Triari"
#define LEGIO_HARD_JOIN_NOT_REGISTERED L"Tylko dla prawdziwych Wyznawców"
#define LEGIO_HARD L"Najbardziej doświadczone siły Imperium. Są podporządkowane woli Imperatora, doskonale wyćwiczone.\n\nWeteranów wysyła się tam, gdzie żadne inne legiony nie mają szans przetrwać. W podzięce, Imperator docenia ich poświęcenie najwyżej jak to możliwe."
#define LEGIO_HARD_NOT_REGISTERED L"\n\n Tylko zarejestrowani użytkownicy mogą dołączyć do elitarnych sił."

// Options
#define OPTIONS_HEADLINE L"Opcje"
#define OPTIONS_SHOW_GRAPHICS L"Grafika"
#define OPTIONS_SHOW_AUDIO L"Dźwięk"
#define OPTIONS_SHOW_CONTROLS L"Kontrolery"

// Controls
#define CONTROLS_HEADLINE L"Kontroler"
#define CONTROLS_SHOW_MOUSE L"Mysz"
#define CONTROLS_SHOW_KEYBOARD L"Klawiatura"
#define CONTROLS_SHOW_GAMEPAD L"Gamepad/Joystick"

// Mouse
#define MOUSE_HEADLINE L"Ustawienia myszy"
#define MOUSE_SENSITIVITY L"Czułość: %.2f"
#define MOUSE_RESET L"Przywróć wartości domyślne"

// Keyboard
#define KEYBOARD_HEADLINE L"Ustawienia klawiatury"
#define KEYBOARD_MOVE_LEFT L"Ruch w lewo: "
#define KEYBOARD_MOVE_RIGHT L"Ruch w prawo: "
#define KEYBOARD_MOVE_UP L"Ruch w górę: "
#define KEYBOARD_MOVE_DOWN L"Ruch w dół: "
#define KEYBOARD_PREV_ABILITY L"Poprzednia zdolność: "
#define KEYBOARD_NEXT_ABILITY L"Następna zdolność: "
#define KEYBOARD_FIRE L"Strzelanie: "
#define KEYBOARD_USE_ABILITY L"Użyj zdolności: "
#define KEYBOARD_SET_RAPIDFIRE L"Szybki ostrzał: "
#define KEYBOARD_SET_REGENERATION L"Regeneracja: "
#define KEYBOARD_SET_TIMEWARP L"Skok nadprzestrzenny: "
#define KEYBOARD_SET_MISSILES L"Rakiety: "
#define KEYBOARD_SENSITIVITY L"Czułość: %.2f"
#define KEYBOARD_RESET L"Przywróć wartości domyślne"

// Controller
#define CONTROLLER_HEADLINE L"Ustawienia kontrolera"
#define CONTROLLER_MOVE_LEFT_RIGHT L"Ruch lewo-prawo: "
#define CONTROLLER_MOVE_UP_DOWN L"Ruch góra-dół: "
#define CONTROLLER_FIRE L"Strzelanie: "
#define CONTROLLER_USE_ABILITY L"Użyj zdolności: "
#define CONTROLLER_SET_RAPIDFIRE L"Szybki ostrzał: "
#define CONTROLLER_SET_REGENERATION L"Regeneracja: "
#define CONTROLLER_SET_TIMEWARP L"Skok nadprzestrzenny: "
#define CONTROLLER_SET_MISSILES L"Rakiety: "
#define CONTROLLER_SENSITIVITY L"Czułość: %.2f"
#define CONTROLLER_RESET L"Przywróć wartości domyślne"

// Audio
#define AUDIO_HEADLINE L"Ustawienia dźwięku"
#define AUDIO_MUSIC_VOLUME L"Głośność muzyki: %.0f"
#define AUDIO_SOUND_VOULME L"Głośność efektów: %.0f"

// Graphics
#define GRAPHICS_HEADLINE L"Ustawienia grafiki"
#define GRAPHICS_SHOW_RESOLUTION L"Rozdzielczość"
#define GRAPHICS_SHOW_MODE L"Tryb okna"

// Resolution
#define RESOLUTION_HEADLINE L"Wybierz rozdzielczość"

// Mode
#define MODE_HEADLINE L"Wybierz tryb okna"
#define MODE_FULLSCREEN L"Pełny ekran"
#define MODE_WINDOWED L"Gra w oknie"

// misc
#define MENU_BUY L"Zakup pełną wersję"
#define MENU_BACK L"Cofnij"

// ingame strings

#define MESSAGE_HEADLINE L"Komunikat Imperium"

#define DEMO_HEADLINE L"Koniec demo"
#define DEMO_TEXT L"To najdalszy z etapów, do którego mogą dotrzeć Ci, którzy nie są prawdziwymi wyznawcami.\n\nPobierz Imperialny Kod Rejestracyjny i skorzystaj z następujących korzyści: \n\n\nDostęp do wszystkich 10 szalonych poziomów\n\n10 wyjątkowych Bosów do zniszczenia\n\nWydłużony czas życia z 5 ekstra życiami\n\nPełen dostęp do Imperialnej bramki skoku nadprzestrzennego\n\n\n\nDarmowe uaktualnienia i poprawki\n\nDostęp do pomocy technicznej!"
#define DEMO_NO_THANKS_TIME L"Nie jestem godzien tego(%i)"
#define DEMO_NO_THANKS L"Nie jestem godzien tego"

#define HIGHSCORE L"Jesteś godzien tego, by zapamiętano Cię jako wiernego sługę Imperatora. \n\nWpisz swoje imię (ESC aby pzerwać):%s"
#define HIGHSCORE_NOT_REGISTERED L"Jesteś godzien tego, by zapamiętano Cię jako wiernego sługę Imperatora.\n\nImperator nagradza Cię dostępem do przedłużonej pomocy technicznej dostępnej dla zarejestrowanych Wyznawców!\n\nWpisz swoje imię (ESC aby przerwać):%s"

#define GAME_OVER L"Game Over!\n\n Wynik: %i\n\nNaciśnij strzał aby kontynuować"
#define GAME_OVER_NOT_REGISTERED L"Koniec gry!\n\nImperator nagradza Cię dostępem do przedłużonej pomocy technicznej dostępnej dla zarejestrowanych Wyznawców!\n\nWynik: %i\n\nNaciśnij strzał aby kontynuować"

#define EPILOGUE_HEADLINE L"Epilog"
#define EPILOGUE_TEXT L"Pozostawieni w spokoju, obcy przybysze powrócili do swoich codziennych zadań zostawiając czas i przestrzeń w spokoju. \nImperialna administracja rozpadła się i ludzie mieszkający w galaktyce mogli powrócić do życia w wolności, z dala od tyranii i ucisku.\n\nGratulacje, udało Ci się ukończyć T.W.T.P.B!\n\n\nPamiętaj, że powinieneś zawsze myśleć samodzielnie\ni\ncenić swoją wolność oraz WOLNOŚĆ innych ludzi\n\nUwierz w swoje marzenia i urzeczywistnią się one!\n\nPokój z Tobą!\n\n\n"