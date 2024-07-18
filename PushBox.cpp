#include <stdio.h>  
//交换字符数组元素  
void swapPosition(char ch[][11],int oldX,int oldY,int newX,int newY)  
{  
    char temp;  
    temp = *(*(ch + oldX) + oldY);  
    *(*(ch + oldX) + oldY) = *(*(ch + newX) + newY);  
    *(*(ch + newX) + newY) = temp;  
      
}  
//打印数组  
void printArray(char (*ch)[11])  
{  
    for(int i = 0;i < 10;i++)  
    {  
        for(int j = 0;j < 10;j++)  
        {  
            printf("%c\t",*(*(ch + i) + j));  
        }  
        printf("\n");  
    }  
}  
int main(int argc, char* argv[])  
{  
    //定义数组  
    char ch[10][11] = {  
        {'#','#','#','#','#','#','#','#','#','#','\0'},  
        {'#','#','#',' ',' ','#','#','#','#','#','\0'},  
        {'#','0','X',' ',' ','#','#','#','#','#','\0'},  
        {'#','#','#','#',' ','#','#','#','#','#','\0'},  
        {'#','#','#',' ',' ',' ',' ','#','#','#','\0'},  
        {'#','#','#',' ',' ',' ',' ',' ','#','#','\0'},  
        {'#','#','#','#','#',' ',' ',' ','#','#','\0'},  
        {'#','#','#','#','#',' ',' ',' ',' ',' ','\0'},  
        {'#','#','#','#','#','#','#','#',' ',' ','\0'},  
        {'#','#','#','#','#','#','#','#','#','#','\0'}  
    };  
    //打印数组  
    printArray(ch);  
    //记录小人及箱子位置  
    //定义路的标志变量street  
    int personX = 2,personY = 1,boxX = 2, boxY = 2;  
    char street = ' ';  
    //提示用户输入移动方向  
    printf("请输入小人移动方向：（w-上，s-下，a-左，d-右）\n");  
    //定义记录用户输入的方向  
    char direction;  
    //根据方向定义循环语句  
    while(1)  
    {  
        scanf("%c",&direction);  
        getchar();//接收键盘回车符  
        switch(direction)  
        {  
            case 'w':  
            case 'W':  
                if(*(*(ch+personX-1)+personY) == street)  
                {  
                    swapPosition(ch,personX,personY,personX - 1,personY );  
                    personX--;  
                      
                }  
                else if(*(*(ch+personX-1)+personY) == *(*(ch+boxX)+boxY))  
                {  
                    if(*(*(ch+boxX-1)+boxY) == street)  
                    {  
                        swapPosition(ch,boxX,boxY,boxX - 1,boxY);  
                        boxX--;  
                        swapPosition(ch,personX,personY,personX - 1,personY);  
                        personX--;  
                    }  
                }  
         
                break;  
            case 's':  
            case 'S':  
                if(*(*(ch+personX + 1) + personY) == street)  
                {  
                    swapPosition(ch,personX,personY,personX + 1,personY );  
                    personX++;  
                      
                }  
                else if(*(*(ch+personX+1)+personY) == *(*(ch+boxX)+boxY))  
                {  
                    if(*(*(ch+boxX+1)+boxY) == street)  
                    {  
                        swapPosition(ch,boxX,boxY,boxX + 1,boxY);  
                        boxX++;  
                        swapPosition(ch,personX,personY,personX + 1,personY);  
                        personX++;  
                    }  
                }  
                break;  
            case 'a':  
            case 'A':  
                if(*(*(ch+personX)+personY - 1) == street)  
                {  
                    swapPosition(ch,personX,personY,personX ,personY - 1 );  
                    personY--;  
                      
                }  
                else if(*(*(ch+personX)+personY - 1) == *(*(ch+boxX)+boxY))  
                {  
                    if(*(*(ch+boxX)+boxY - 1) == street)  
                    {  
                        swapPosition(ch,boxX,boxY,boxX,boxY - 1);  
                        boxY--;  
                        swapPosition(ch,personX,personY,personX,personY - 1);  
                        personY--;  
                    }  
                }  
                break;  
            case 'd':  
            case 'D':  
                
                if(*(*(ch+personX)+personY + 1) == street)  
                {  
                    swapPosition(ch,personX,personY,personX ,personY + 1 );  
                    personY++;  
                      
                }  
                else if(*(*(ch+personX)+personY + 1) == *(*(ch+boxX)+boxY))  
                {  
                    if(*(*(ch+boxX)+boxY + 1) == street)  
                    {  
                        swapPosition(ch,boxX,boxY,boxX,boxY + 1);  
                        boxY++;  
                        swapPosition(ch,personX,personY,personX,personY + 1);  
                        personY++;  
                    }  
                }  
                break;  
                case 'q':  
                case 'Q':  
                return 0;  
              
        }  
        printArray(ch);  
        //定义结束标志  
        if (boxY == 9) {  
            printf("恭喜你，游戏过关！\n");  
            return 0;  
        }  
    }  
  
    return 0;  
}  
