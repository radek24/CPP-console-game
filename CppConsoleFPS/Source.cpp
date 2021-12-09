// CppConsoleFPS.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include <iostream>
#include <Windows.h>
#include <chrono>
#include<string> 

using namespace std;


enum PIXEL_TYPE
{
    PIXEL_SOLID = 0x2588,
    PIXEL_THREEQUARTERS = 0x2593,
    PIXEL_HALF = 0x2592,
    PIXEL_QUARTER = 0x2591,
};


int xScreenWidth= 180;
int yScreenHeight =60;
float fScore = 0;
float fMaxScore = 0;
float fSpeed = 20.0f;
float fMultiplier = 1.0f;

void clearScreen(wchar_t* activeScreen);
void printMessageToScreen(string message, int startX, int startY, wchar_t* activeScreen);

class MyVector2D{
public:
    float X;
    float Y;
    MyVector2D(float aX, float aY) {
        X = aX;
        Y = aY;
    }
};


class Box {
public:
    float fWidth;
    float fHeight;
    float PosX;
    float PosY;
    short nShade = PIXEL_SOLID;
    bool Killer = false;
    wchar_t* activeScreen;

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
            for (int y = (int)PosY; y < (int)(PosY + fHeight); y++)
            {
                if (y < 0 || y > yScreenHeight || x < 0 || x  > xScreenWidth) {
                    printMessageToScreen("OBJEKT MIMO OBRAZOVKU", 10, 10, activeScreen);

                }
                else {
                    activeScreen[x + (xScreenWidth)*y] = nShade;

                }
            }
        }
    }
};

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
            printMessageToScreen("OBJEKT MIMO OBRAZOVKU", 10, 10, activeScreen);

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

    void CheckBoxColision(Box colider) {
        if (!(fPlayerX+0.1< colider.PosX || fPlayerX- 0.1 > colider.PosX + colider.fWidth || fPlayerY+0.1 < colider.PosY || fPlayerY - 0.1 > colider.PosY + colider.fHeight)) {
            if(colider.Killer == false){
            
            if (fPlayerX > colider.PosX && fPlayerX < colider.PosX + (colider.fWidth/2) && fPlayerY > colider.PosY+0.1 && fPlayerY < colider.PosY + colider.fHeight - 0.1) {
                fPlayerX = colider.PosX-0.1;
            }
            if (fPlayerX < colider.PosX + colider.fWidth && fPlayerX > colider.PosX + (colider.fWidth / 2.0)&& fPlayerY > colider.PosY + 0.1 && fPlayerY < colider.PosY + colider.fHeight - 0.1) {
                fPlayerX = colider.PosX+ colider.fWidth + 0.1;
            }
            if (fPlayerY > colider.PosY && fPlayerY < colider.PosY+(colider.fHeight/2.0) && fPlayerX > colider.PosX+0.1 && fPlayerX < colider.PosX + colider.fWidth - 0.1) {
                fPlayerY = colider.PosY - 0.1;
            }
            if (fPlayerY < colider.PosY + colider.fHeight && fPlayerY > colider.PosY + (colider.fHeight / 2.0) && fPlayerX > colider.PosX + 0.1 && fPlayerX < colider.PosX + colider.fWidth - 0.1) {
                fPlayerY = colider.PosY+colider.fHeight + 0.1;
            }
            }
            else {
                printMessageToScreen("MRTVY", 10, 10, activeScreen);

            }
        }
    }
};



int main()
{

    wchar_t* screen = new wchar_t[xScreenWidth * yScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();
    auto strtime = chrono::system_clock::now();
    Player Player1(20.0f,30.0f,screen);
    Box wall1(xScreenWidth, 2.0, 0, 8.0, screen);
    Box wall2(xScreenWidth, 2.0, 0, 50.0, screen);
    float Top = 8.0 + 2.0;
    float Bottom = 50.0;
    
    Box Obstacle1(5.00, 20.0, 130.0, Top,screen,PIXEL_THREEQUARTERS);
    Obstacle1.Killer = true;


    while (1) {
        
        //DeltaTime
        auto tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fDeltaTime = elapsedTime.count();
        
        //Time from start
        auto CHtime = chrono::system_clock::now();
        chrono::duration<float> TimeStart = CHtime- strtime;
        float fTime = TimeStart.count();
        
        
        //kontrola
       
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
        wall1.Draw();
        wall2.Draw();
        
        
        
        
        
        
        Obstacle1.Draw();

        Player1.CheckBorderCollisions();
        Player1.CheckBoxColision(Obstacle1);
        Player1.CheckBoxColision(wall1);
        Player1.CheckBoxColision(wall2);
        fMultiplier += 0.2f * fDeltaTime;
        fScore +=1.0f*fDeltaTime* fMultiplier;
        
        if (fScore > fMaxScore)
        {
            fMaxScore = fScore;
        }


        printMessageToScreen("PlayerX: "+ std::to_string(Player1.fPlayerX), 1, 1, screen);
        printMessageToScreen("PlayerY: " + std::to_string(Player1.fPlayerY), 1, 2, screen);
        printMessageToScreen("FPS: " + std::to_string(1.0f/fDeltaTime), 1, 3, screen);
        printMessageToScreen("Time: " + std::to_string(fTime), 1, 4, screen);
        
        printMessageToScreen("---------------------", 50, 3, screen);
        printMessageToScreen("Your score: " + std::to_string((int)round(fScore)), 50, 4, screen);
        printMessageToScreen("Your max score: " + std::to_string((int)round(fMaxScore)), 50, 5, screen);
        printMessageToScreen("---------------------", 50, 6, screen);

        screen[xScreenWidth * yScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, xScreenWidth * yScreenHeight, { 0,0 }, &dwBytesWritten);
        
    }
    return 0;
}




void printMessageToScreen(string message, int startX, int startY, wchar_t* activeScreen) {
    for (int i = 0; i < message.length(); i++)
    {
        activeScreen[(startX+i) + (xScreenWidth)*startY] = message[i];
    }
}
void clearScreen(wchar_t* activeScreen) {
    for (int x = 0; x < xScreenWidth; x++)
    {
        for (int y = 0; y < yScreenHeight; y++)
        {
            activeScreen[y * xScreenWidth + x] = ' ';
        }
    }
}
