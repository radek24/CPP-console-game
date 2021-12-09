#include <iostream>
#include <Windows.h>
#include <chrono>
#include <string> 
#include <time.h> 
#include <fstream>
//lol
#define getrandom(min,max) rand()%(max-min+1)+min

using namespace std;


enum PIXEL_TYPE
{
    PIXEL_SOLID = 0x2588,
    PIXEL_THREEQUARTERS = 0x2593,
    PIXEL_HALF = 0x2592,
    PIXEL_QUARTER = 0x2591,
};

//Globální promene
int xScreenWidth= 180;
int yScreenHeight =60;
float fScore = 0;
float fMaxScore = 0;
float fSpeed = 20.0f;
float fMultiplier = 1.0f;
float fDifficulty = 10.0f;
bool Bgameloop = true;
bool BMainMenu = true;
bool BShowCase = false;
bool BconfirmMenu = false;

//deklarace nebo definice funkci, nevim nejsem programator
void clearScreen(wchar_t* activeScreen);
void printMessageToScreen(string message, int startX, int startY, wchar_t* activeScreen);

//class box a jeho metody
class Box {
public:
    float fWidth;
    float fHeight;
    float PosX;
    float PosY;
    short nShade = PIXEL_SOLID;
    bool Killer = false;
    wchar_t* activeScreen;

private:
    //promena ktera se odcita kdyz neco scrolluje, asi brzo pretece idk
    float scroll = -300;
public:
    //konstruktory
    Box(float AfWidth, float AfHeight, float APosX, float APoxY, wchar_t* AactiveScreen) {
        fWidth = AfWidth;
        fHeight = AfHeight;
        PosX = APosX;
        PosY = APoxY;
        activeScreen = AactiveScreen;
    }
    Box(float AfWidth, float AfHeight, float APosX, float APoxY, wchar_t* AactiveScreen , short AnShade) {
        fWidth = AfWidth;
        fHeight = AfHeight;
        PosX = APosX;
        PosY = APoxY;
        nShade = AnShade;
        activeScreen = AactiveScreen;
    }
    void Shade(short AnShade) {
        nShade = AnShade;
    }
    void Draw() {
        for (int x = (int)PosX; x < (int)(PosX + fWidth); x++)
        {
            //posefene vykreslovani at to nezasahuje mimo vyalokovanou pamet
            for (int y = (int)PosY; y < (int)(PosY + fHeight); y++)
            {
                if (!(y < 0 || y > yScreenHeight || x < 0 || x  > xScreenWidth)) {
                    activeScreen[x + (xScreenWidth)*y] = nShade;
                }
            }
        }
    }
    
    void DrawChecker(float AfDeltaTime) {
        scroll -= fDifficulty * AfDeltaTime;
        for (int x = (int)PosX; x < (int)(PosX + fWidth); x++)
        {
            for (int y = (int)PosY; y < (int)(PosY + fHeight); y++)
            {
                if (!(y < 0 || y > yScreenHeight || x < 0 || x  > xScreenWidth)) {
                    if (((int)(x + scroll) % 2 == 0 && y % 2 != 0) || ((int)(x + scroll) % 2 != 0 && y % 2 == 0)) {
                        nShade = PIXEL_THREEQUARTERS;
                    }
                   
                    //tohle vyhresluje cary, mozna nekdy pouziju tf
                    /* if ((int)(x + scroll) % 2 == 0) {
                        nShade = PIXEL_THREEQUARTERS;
                   }*/
                    else {
                        nShade = PIXEL_SOLID;
                    }
                    
                    activeScreen[x + (xScreenWidth)*y] = nShade;
                }
            }
        }
    }
    //tuhle metodu myslim ani nepouzivam, ale jednou se bude hodit
    void SetPosition(float aPositionX, float aPositionY) {
         PosX= aPositionX;
         PosY= aPositionY;
    }
    //specialni metoda pro hru
    bool isOutHorizontalBounds() {
        if (PosX + fWidth < -1.0f) {
            return true;
        }
        else {
            return false;
        }
    }
    //posouva prekazku doleva, velice genericka metoda vim
    void ObstacleMove(float AfDeltaTime) {
        PosX -= fDifficulty * AfDeltaTime;
    }
};
//class hrac
class Player {
public:
    float fPlayerX;
    float fPlayerY;
    short nShade= PIXEL_HALF;
    wchar_t* activeScreen;
    Player(float AfPlayerX, float AfPlayerY, wchar_t* AactiveScreen) {
        fPlayerX = AfPlayerX;
        fPlayerY = AfPlayerY;
        activeScreen = AactiveScreen;
    }
    Player(float AfPlayerX, float AfPlayerY, wchar_t* AactiveScreen,  short AnShade) {
        fPlayerX = AfPlayerX;
        fPlayerY = AfPlayerY;
        nShade = AnShade;
        activeScreen = AactiveScreen;
    }


    void Shade(short AnShade) {
        nShade = AnShade;
    }
    void Draw() {
        if (fPlayerY < 0 || fPlayerY > yScreenHeight || fPlayerX < 0 || fPlayerX > xScreenWidth) {
        }
        else {
            activeScreen[(int)fPlayerX + ((xScreenWidth) * (int)fPlayerY)] = nShade;
        }
    }

    
    void CheckBorderCollisions() {
        if (fPlayerX < 0.2f) {
            fPlayerX = 0.3f;
        }
        if (fPlayerY < 0.2f) {
            fPlayerY = 0.3f;
        }
        if (fPlayerX > xScreenWidth - 0.2f) {
            fPlayerX = xScreenWidth-0.5f ;
        }
        if (fPlayerY > yScreenHeight -0.2f) {
            fPlayerY = yScreenHeight - 0.5f;
        }


    }


    //Genericka metoda pro kontrolu kolizi, legit to zabralo tak 3 hodiny a ani to poradne neefunguje :(
    void CheckBoxColisionArr(Box colider[], int Asize) {
        //fici pro kazdy objekt v array
        for (int i = 0; i < Asize; i++)
        {
            //prvni se zkontroluje jestli je hrac nekde v kolizi, pro kill
            if (!(fPlayerX + 0.2 < colider[i].PosX || fPlayerX - 0.2 > colider[i].PosX + colider[i].fWidth || fPlayerY + 0.2 < colider[i].PosY || fPlayerY - 0.2 > colider[i].PosY + colider[i].fHeight)) {
                
                if (colider[i].Killer == false) {
                    //potom se kontroluje z jake strany narazil a pak ho šupne zpatky
                    if (fPlayerX+0.3f > colider[i].PosX && fPlayerX < colider[i].PosX + (colider[i].fWidth / 2) && fPlayerY > colider[i].PosY + 0.2 && fPlayerY < colider[i].PosY + colider[i].fHeight - 0.2) {
                        fPlayerX = colider[i].PosX - 0.5f;
                    }
                    if (fPlayerX < colider[i].PosX + colider[i].fWidth && fPlayerX > colider[i].PosX + (colider[i].fWidth / 2.0) && fPlayerY > colider[i].PosY + 0.2 && fPlayerY < colider[i].PosY + colider[i].fHeight - 0.2) {
                        fPlayerX = colider[i].PosX + colider[i].fWidth + 0.2f;
                    }
                    if (fPlayerY > colider[i].PosY && fPlayerY < colider[i].PosY + (colider[i].fHeight / 2.0) && fPlayerX > colider[i].PosX + 0.2 && fPlayerX < colider[i].PosX + colider[i].fWidth - 0.2) {
                        fPlayerY = colider[i].PosY - 0.2;
                    }
                    if (fPlayerY < colider[i].PosY + colider[i].fHeight && fPlayerY > colider[i].PosY + (colider[i].fHeight / 2.0) && fPlayerX > colider[i].PosX + 0.2 && fPlayerX < colider[i].PosX + colider[i].fWidth - 0.2) {
                        fPlayerY = colider[i].PosY + colider[i].fHeight + 0.2;
                    }
                }
                else {
                    //pokud se ma hrac zabit ukonci gameloop
                    Bgameloop = false;
                }
            }
        }     
    }
};



int main()
{
    //otevreni nebo vytvoreni souboru debug.log, kde se uklada skore
    string StoredMaxScoreString;
    std::ifstream DebugLog;
    DebugLog.open ("debug.txt");
    
    //pokud soubor neexistuje tak ho proste vytvor lol
    if (!DebugLog) {
        //kreativni nazvy promenych smile
        ofstream NothingSmile("debug.txt");
        NothingSmile.close();
        DebugLog.open("debug.txt");
    }
    
    if (DebugLog.is_open()) {
        std::getline(DebugLog, StoredMaxScoreString);
        //tohle by nemelo fungovat, ale funguje tak do toho nehrabu
        float StoredMaxScoreFloat=0.0;
        if (StoredMaxScoreString.find(".") != std::string::npos) {
            StoredMaxScoreFloat = std::stof(StoredMaxScoreString);
            fMaxScore = StoredMaxScoreFloat;
        }
        else {
            fMaxScore = 0.0;
        }

    }

    DebugLog.close();
    //sranda time nula
    srand((unsigned)time(0));

    //inicializace konzole a screenbufferu, ktery 1D array
    wchar_t* screen = new wchar_t[xScreenWidth * yScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    //promene pro cas a deltatime
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();
    auto strtime = chrono::system_clock::now();
    
    //definice objektu pro showcase level
    Player ShowcasePlayer(10.0f, 20.0f, screen);
    Box ShowcaseMovingBox(10.0f, 10.0f, 30.0f, 30.0f, screen);
    Box ShowcaseCheckerBox(10.0f, 10.0f, 60.0f, 10.0f, screen);
    Box ShowcaseRotatingBox(10.0f, 5.0f, 100.0f, 20.0f, screen);
    Box ShowcaseBox2(10.0f, 5.0f, 150.0f, 20.0f, screen,'#');
    Box ShowcaseSrollingBox(20.0f, 5.0f, 150.0f, 40.0f, screen);

    //definice objektu pro hru
    Player Player1(10.0f,20.0f,screen);
    
    Box wall1(xScreenWidth, 2.0, 0, 8.0, screen);
    Box wall2(xScreenWidth, 2.0, 0, 30.0, screen);
    Box wall3(2.0, 22.0, -1.0, 8.0, screen);
    
    //promene pro ulehceni prace
    double Top = 8.0 + 2.0;
    double Bottom = 30.0;
    double start = 130.0;
    float step = 37.0f;

    
    Box ObstacleTop(5.00, 1.0, start, Top,screen,PIXEL_THREEQUARTERS);
    Box ObstacleBottom(5.00, 10.0, start, Bottom - 10.0, screen, PIXEL_THREEQUARTERS);

    Box ObstacleTop2(5.00, 5.0, start+(step), Top, screen, PIXEL_THREEQUARTERS);
    Box ObstacleBottom2(5.00, 6.0, start + (step), Bottom - 6.0, screen, PIXEL_THREEQUARTERS);

    Box ObstacleTop3(5.00, 8.0, start + (step*2), Top, screen, PIXEL_THREEQUARTERS);
    Box ObstacleBottom3(5.00, 4.0, start + (step*2), Bottom - 4.0, screen, PIXEL_THREEQUARTERS);

    Box ObstacleTop4(5.00, 12.0, start + (step*3), Top, screen, PIXEL_THREEQUARTERS);
    Box ObstacleBottom4(5.00, 5.0, start + (step*3), Bottom - 5.0, screen, PIXEL_THREEQUARTERS);
   
    Box ObstacleTop5(5.00, 5.0, start + (step * 4), Top, screen, PIXEL_THREEQUARTERS);
    Box ObstacleBottom5(5.00, 5.0, start + (step * 4), Bottom - 5.0, screen, PIXEL_THREEQUARTERS);
    
    //array vsech obstacles a jejich pocet, hodne ulehci praci
    int ObstaclesSize = 10;
    Box Obstacles[] = { ObstacleTop,ObstacleBottom,ObstacleTop2,ObstacleBottom2,ObstacleTop3,ObstacleBottom3,ObstacleTop4,ObstacleBottom4,ObstacleTop5,ObstacleBottom5 };
    //ulehceni prace zde
    for (int i = 0; i < ObstaclesSize; i++)
    {
        Obstacles[i].Killer = true;
    }
    //General game loop, cele to resim nekolika loopy za sebou ktere se postupne vypinaji nebo zapinaji podle toho co hrac macka, ROZHODNE to neni nejlepsi reseni a 
    //velice spatne se scaluje, ale nic lepsiho me v tu chvili nenapadlo.

    //Rendorvani se resi tak, ze se pise do 1D arraye (screen buffer), to co je pozdeji v kodu se zobrazi nad tim co je pred v kodu
    while (1){
        //Main menu loop
        while (BMainMenu) {
            clearScreen(screen);
            printMessageToScreen(" C++ game ", 5, 2, screen);
            printMessageToScreen("----------", 5, 3, screen);
            
            printMessageToScreen("Press P to play game", 7, 5, screen);
            printMessageToScreen("Press U clear your best score", 7, 6, screen);
            printMessageToScreen("Press L to enter showcase level", 7, 7, screen);
            printMessageToScreen("Press E to exit game", 7, 8, screen);
            
            //klavesy funguji jen kdyz neni zapnute confirm menu
            if (!BconfirmMenu) {
                if (GetAsyncKeyState((unsigned short)'P') & 0x8000)
                    BMainMenu = false;
                if (GetAsyncKeyState((unsigned short)'L') & 0x8000)
                    BMainMenu = false;
                if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
                    return 0;
                BShowCase = true;
            }
            
            
            if (GetAsyncKeyState((unsigned short)'U') & 0x8000)
            {
                BconfirmMenu = true;
            }
            
            //jsem profesionalni grafik, takze tomu odpovida ui tehle hry :sungllases:
            if (BconfirmMenu) {
                printMessageToScreen("----------------------------------------", 20, 10, screen);
                printMessageToScreen("I            ARE YOU SURE?             I", 20, 11, screen);
                printMessageToScreen("I         to delete best score         I", 20, 12, screen);
                printMessageToScreen("I                                      I", 20, 13, screen);
                printMessageToScreen("I             [Y]-yes                  I", 20, 14, screen);
                printMessageToScreen("I             [N]-no                   I", 20, 15, screen);
                printMessageToScreen("I                                      I", 20, 16, screen);
                printMessageToScreen("I                                      I", 20, 17, screen);
                printMessageToScreen("I                                      I", 20, 18, screen);
                printMessageToScreen("I                                      I", 20, 19, screen);
                printMessageToScreen("----------------------------------------", 20, 20, screen);

                //kdyz chce tak to proste smaz
                if (GetAsyncKeyState((unsigned short)'Y') & 0x8000) {
                    std::ofstream DebugLogInput;
                    DebugLogInput.open("debug.txt", std::ofstream::out | std::ofstream::trunc);
                    DebugLogInput << "";
                    BconfirmMenu = false;
                }
                if (GetAsyncKeyState((unsigned short)'N') & 0x8000) {
                    BconfirmMenu = false;
                }
            }

            //vypsani celeho screen bufferu do konzole, opakuje se na konci kazdeho gameloopu
            screen[xScreenWidth * yScreenHeight - 1] = '\0';
            WriteConsoleOutputCharacter(hConsole, screen, xScreenWidth * yScreenHeight, { 0,0 }, &dwBytesWritten);
        }
        while (BShowCase) {
            //DeltaTime
            auto tp2 = chrono::system_clock::now();
            chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;
            float fDeltaTime = elapsedTime.count();

            //Time from start
            auto CHtime = chrono::system_clock::now();
            chrono::duration<float> TimeStart = CHtime - strtime;
            float fTime = TimeStart.count();


            //ovladani
            if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
                ShowcasePlayer.fPlayerX -= fSpeed * fDeltaTime;
            if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
                ShowcasePlayer.fPlayerX += fSpeed * fDeltaTime;
            if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
                ShowcasePlayer.fPlayerY -= fSpeed * fDeltaTime;
            if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
                ShowcasePlayer.fPlayerY += fSpeed * fDeltaTime;
            if (GetAsyncKeyState((unsigned short)'P') & 0x8000) {
                BShowCase = false;
            }
            

            clearScreen(screen);
            
            
            //hybajici se ctverecky
            ShowcaseMovingBox.PosX = 30.0+sinf(fTime)*10.0f;
            ShowcasePlayer.Draw();
           ShowcaseRotatingBox.PosY = 20.0 + sinf(fTime*0.5f) * 10.0f;
            ShowcaseRotatingBox.PosX = 100.0 + cosf(fTime * 0.5f) * 10.0f;

            //vsechny objekty se ukladaji do arraye a pak se vykresi
            Box Walls[] = { ShowcaseMovingBox,ShowcaseCheckerBox,ShowcaseRotatingBox,ShowcaseBox2,ShowcaseSrollingBox };
            int size = 5;
            for (int p = 0; p < size; p++)
            {
                Walls[p].Draw();
            }
            //Basically overide drawcallu nahore
            ShowcaseCheckerBox.DrawChecker(0.0);
            ShowcaseSrollingBox.DrawChecker(fDeltaTime);
            
            //checknuti kolizi
            ShowcasePlayer.CheckBoxColisionArr(Walls, size);
            ShowcasePlayer.CheckBorderCollisions();
            
            //nejake debug zpravy
            printMessageToScreen(" PlayerX: " + std::to_string(ShowcasePlayer.fPlayerX), 1, 1, screen);
            printMessageToScreen(" PlayerY: " + std::to_string(ShowcasePlayer.fPlayerY), 1, 2, screen);
            printMessageToScreen(" FPS: " + std::to_string(1.0f / fDeltaTime), 1, 3, screen);
            printMessageToScreen(" Time: " + std::to_string(fTime), 1, 4, screen);
            printMessageToScreen(" [P] to enter game ", 1, 5, screen);
     
            screen[xScreenWidth * yScreenHeight - 1] = '\0';
            WriteConsoleOutputCharacter(hConsole, screen, xScreenWidth * yScreenHeight, { 0,0 }, &dwBytesWritten);
    }


        //hlavni hra loop
    while (Bgameloop) {
        //DeltaTime
        auto tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fDeltaTime = elapsedTime.count();

        //Time from start
        auto CHtime = chrono::system_clock::now();
        chrono::duration<float> TimeStart = CHtime - strtime;
        float fTime = TimeStart.count();
        

        //ovladani
       
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            Player1.fPlayerX -= fSpeed * fDeltaTime;
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            Player1.fPlayerX += fSpeed * fDeltaTime;
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
            Player1.fPlayerY -= fSpeed * fDeltaTime;
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
            Player1.fPlayerY += fSpeed * fDeltaTime;
        clearScreen(screen);

        Player1.Draw();
        
        //postupne zvyseni obtiznosti, vpodstate to je jen ryhlost prekazek
        if (fDifficulty < 45.0f) {
            fDifficulty += 0.25f * fDeltaTime;
        }
        
        //pro kazdou prekazku zkontroluj, jestli uz je mimo mapu a jestli ano hod ji na zacatek a taky ji vygeneruj velikost a sirku diry a nakonec vse vykresli

        for (int h = 0; h < ObstaclesSize; h++)
        {
            Obstacles[h].ObstacleMove(fDeltaTime); 
            if (h % 2 == 0) {
                if (Obstacles[h].isOutHorizontalBounds()) {
                    Obstacles[h].PosX = xScreenWidth + 1.0f;
                    Obstacles[h + 1].PosX = xScreenWidth + 1.0f;

                    float holePos;
                    float holeSize;
                    holePos = getrandom(1, 16);
                    Obstacles[h].fHeight = holePos;
                    int maxhole = 8;
                    if (holePos + maxhole > 20) {
                        maxhole = 4;
                    }
                    holeSize = getrandom(3, maxhole);
                    Obstacles[h+1].PosY = holePos + holeSize + 10.0;
                    Obstacles[h + 1].fHeight = 20 - (holePos + holeSize);
                }
            }
            Obstacles[h].Draw();
        }

        //drawcally zdi
        wall1.Draw();
        wall2.DrawChecker(fDeltaTime);
        wall3.Draw();
        
        Box Walls[] = { wall1,wall2,wall3};
        
        //kolize a jine srandy
        Player1.CheckBoxColisionArr(Walls, 3);
        Player1.CheckBoxColisionArr(Obstacles, ObstaclesSize);
        Player1.CheckBorderCollisions();
        
        
        //score a max score
        fMultiplier += 0.2f * fDeltaTime;
        fScore +=1.0f*fDeltaTime* fMultiplier;
        
        if (fScore > fMaxScore)
        {
            fMaxScore = fScore;
        }
        

        printMessageToScreen("FPS: " + std::to_string(1.0f/fDeltaTime), 1, 1, screen);
        printMessageToScreen("Time: " + std::to_string(fTime), 1, 2, screen);

        printMessageToScreen("---------------------", 50, 1, screen);
        printMessageToScreen("Your score: " + std::to_string((int)round(fScore)), 50, 2, screen);
        printMessageToScreen("Difficulty: " + std::to_string(fDifficulty), 50, 3, screen);
        printMessageToScreen("Your max score: " + std::to_string((int)round(fMaxScore)), 50, 4, screen);
        printMessageToScreen("---------------------", 50, 5, screen);

        screen[xScreenWidth * yScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, xScreenWidth * yScreenHeight, { 0,0 }, &dwBytesWritten);
        
    }
    //DeltaTime
    auto tp2 = chrono::system_clock::now();
    chrono::duration<float> elapsedTime = tp2 - tp1;
    tp1 = tp2;
    float fDeltaTime = elapsedTime.count();

    //Time from start
    auto CHtime = chrono::system_clock::now();
    chrono::duration<float> TimeStart = CHtime - strtime;
    float fTime = TimeStart.count();
    printMessageToScreen("FPS: " + std::to_string(1.0f / fDeltaTime), 1, 1, screen);
    printMessageToScreen("Time: " + std::to_string(fTime), 1, 2, screen);


    //lol prohral slabko

    printMessageToScreen("                                 ", 65, 13, screen);
    printMessageToScreen(" ------------------------------- ", 65, 14, screen);
    printMessageToScreen(" I                             I ", 65, 15, screen);
    printMessageToScreen(" I          YOU LOST           I ", 65, 16, screen);
    printMessageToScreen(" I                             I ", 65, 17, screen);
    printMessageToScreen(" I   Press R to restart game   I ", 65, 18, screen);
    printMessageToScreen(" I     Press E to exit game    I ", 65, 19, screen);
    printMessageToScreen(" I     Press M to main menu    I ", 65, 20, screen);
    printMessageToScreen(" ------------------------------- ", 65, 21, screen);
    printMessageToScreen("                                 ", 65, 22, screen);
    
    
    //restartuj hru a uloz maxscore do souboru
    if (GetAsyncKeyState((unsigned short)'R') & 0x8000) {
        

        std::ofstream DebugLogInput;
        DebugLogInput.open("debug.txt", std::ofstream::out | std::ofstream::trunc);
        DebugLogInput << std::to_string(fMaxScore);

        Bgameloop = true;
    }
    //ficime do menu a ukladame score
    if (GetAsyncKeyState((unsigned short)'M') & 0x8000) {


        std::ofstream DebugLogInput;
        DebugLogInput.open("debug.txt", std::ofstream::out | std::ofstream::trunc);
        DebugLogInput << std::to_string(fMaxScore);
        Bgameloop = true;
        BMainMenu = true;
    }
    
    //hrac hru vzdava a uklada sve spatne score
    if (GetAsyncKeyState((unsigned short)'E') & 0x8000) {

        std::ofstream DebugLogInput;
        DebugLogInput.open("debug.txt", std::ofstream::out | std::ofstream::trunc);
        DebugLogInput << std::to_string(fMaxScore);
        
        return 0;
    }
       
    //restart vsech promenych
    fScore = 0;
    Player1.fPlayerX = 10.0;
    Player1.fPlayerY = 20.0;
    fMultiplier = 1.0f;
    fDifficulty = 10.0f;
    

    //TOHLE JSEM FAKT ZKOUSEL UDELAT V LOOPU, ALE NEFUNGOVALO TO :(
    Obstacles[0].PosX = start + (step * 0);
    Obstacles[1].PosX = start + (step * 0);

    Obstacles[2].PosX = start + (step * 1);
    Obstacles[3].PosX = start + (step * 1);

    Obstacles[4].PosX = start + (step * 2);
    Obstacles[5].PosX = start + (step * 2);

    Obstacles[6].PosX = start + (step * 3);
    Obstacles[7].PosX = start + (step * 3);

    Obstacles[8].PosX = start + (step * 4);
    Obstacles[9].PosX = start + (step * 4);


    screen[xScreenWidth * yScreenHeight - 1] = '\0';
    WriteConsoleOutputCharacter(hConsole, screen, xScreenWidth* yScreenHeight, { 0,0 }, & dwBytesWritten);
    }
    return 0;
}



//funkce ktera vypise jakykoliv string na obrazovku, 
void printMessageToScreen(string message, int startX, int startY, wchar_t* activeScreen) {
    for (int i = 0; i < message.length(); i++)
    {
        activeScreen[(startX+i) + (xScreenWidth)*startY] = message[i];
    }
}

//vycisti celou obrazovku
void clearScreen(wchar_t* activeScreen) {
    for (int x = 0; x < xScreenWidth; x++)
    {
        for (int y = 0; y < yScreenHeight; y++)
        {
            activeScreen[y * xScreenWidth + x] = ' ';
        }
    }
}

//konec zvonec