#include <algorithm>
#include <memory>

class Stream{
public:
    typedef std::shared_ptr<Stream> ptr;

    virtual ~Stream(){}

    /**
     * @brief 读数据
     * @param[out] buffer 接收数据的内存
     * @param[in] length 接收数据的内存大小
     * @return
     *      @retval >0 返回接收到的数据的实际大小
     *      @retval =0 被关闭
     *      @retval <0 出现流错误
     */
    virtual int read(void* buffer, size_t length) = 0;

};