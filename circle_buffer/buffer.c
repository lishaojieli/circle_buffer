/*
数据包格式：[包头：0xAA][数据长度：1字节][数据：n字节][校验和：1字节] 
数据包长度=1字节包头 + 1字节数据长度 + n字节数据 + 1字节校验和
*/


#include <stdio.h>

#define BUFFER_SIZE 128    //循环缓冲区大小

uint8_t buffer[BUFFER_SIZE];  //缓冲区
uint8_t readIndex = 0;        //读索引
uint8_t writeIndex = 0;       //写索引

/*
 * @brief 获取缓冲区内未处理的数据的长度
 * @return 未处理数据的长度
 * @retval 0 缓冲区为空
 * @retval BUFFER_SIZE
 * @retval 1~BUFFER_SIZE-1 未处理数据长度
 */
uint8_t Buffer_NewDataLength(void)
{
    if(readIndex == writeIndex)   //缓冲区为空，原因是默认写入速度大于读出速度且wirteIndex的上限为readIndex-1
    {
        return 0 ;
    }
    if(writeIndex + 1 == readIndex || writeIndex == BUFFER_SIZE - 1 && readIndex == 0)  //缓冲区满
    {
        return BUFFER_SIZE;
    }
    if(readIndex < writeIndex)   //缓冲区未满但不空
    {
        return writeIndex - readIndex;
    }
    else
    {
        return BUFFER_SIZE - readIndex + writeIndex;
    }
}

/*
 * @brief 获取缓冲区剩余空间
 * @return 缓冲区剩余空间
 */
uint8_t Buffer_Remain(void)
{
    return BUFFER_SIZE - Buffer_NewData_Length();
}

/*
 * @brief 向缓冲区写入数据
 * @param data 要写入数据的指针
 * @param length 要写入数据的长度
 * @return 写入数据的长度
 * @retval 0 缓冲区已满无法写入
 * @retval length 写入数据的长度
 */
uint8_t Buffer_Write(uint8_t *data, uint8_t length)
{
    if(Buffer_Remain() == 0)  //缓冲区已满
    {
        return 0;
    }
    if(writeIndex + length <= BUFFER_SIZE -1)
    {
        memcpy(buffer + writeIndex, data, length);
        writeIndex += length;
    }
    else
    {
        uint8_t firstLength = BUFFER_SIZE - writeIndex;
        memcpy(buffer + writeIndex, data, firstLength);
        memcpy(buffer, data + firstLength, length - firstLength);
        writeIndex = length - firstLength;
    }
    return length;
}

/*
 * @brief 从缓冲区读取一个数据
 * @param i 第i个数据
 * @return 缓冲区的第i个数据
 */
uint8_t Buffer_Read(uint8_t i)
{
    uint8_t index = i % BUFFER_SIZE;   
    return buffer[index];
}

/*
 *@brief 增加读索引
 *@param length 增加读索引的长度
 */
void Buffer_AddReadindex(uint8_t length)
{
    readIndex += length;
    readIndex % BUFFER_SIZE;
}

/*
 *@brief 从缓冲区读取数据包
 *@param NewData 读出的数据存放的地址指针
 *@return 读出的数据
 *@retval 0 数据包长度不合法
 *@retval length 成功读出的数据包长度
 */
uint8_t Buffer_GetBag(uint8_t *NewData)
{
    while(1)
    {
        if(Buffer_NewDataLength() < 4)   //数据包长度小于最小包长度
        {
            return 0;
        }
        if(Buffer_Read(readIndex) != 0xAA)   //没找到包头
        {
            Buffer_AddreadIndex(1);
            continue;
        }
        uint8_t length = Buffer_Read(readIndex + 1);  //数据包不完整
        if(Buffer_NewDataLength() < length)
        {
            return 0;
        }
        uint8_t sum = 0;
        for(uint8_t i = 0; i < length - 1; i++)
        {
            sum += Buffer_Read(readIndex + i);
        }
        if(sum != Buffer_readIndex(readIndex + length - 1))   //校验失败
        {
            Bffer_AddreadIndex(1);
            continue;
        }
        for(uint8_t i = 0; i < length; i++)   //校验成功
        {
            NewData[i] = Buffer_Read(readIndex + i);
        }
        Buffer_AddreadIndex(length);
        return length;
    }
}
