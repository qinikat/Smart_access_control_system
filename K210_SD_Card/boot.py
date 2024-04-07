import sensor,image,lcd  # import 相关库
import KPU as kpu   #模型库
import time         #定时库
import ubinascii    #二进制库
import uos          #文件库
from Maix import FPIOA,GPIO     #GPIO库
from fpioa_manager import fm    #标准库
from machine import UART        #串口库
from machine import Timer       #定时器库
from machine import WDT         #看门狗库

fm.register(9,fm.fpioa.UART1_TX)#串口引脚映射
fm.register(10,fm.fpioa.UART1_RX)
fm.register(15, fm.fpioa.GPIO0)
#fm.register(9, fm.fpioa.GPIOHS9)

com = UART(UART.UART1, 115200, timeout=50, read_buf_len=4096)#构建串口对象

#GPIO9 = GPIO(GPIO.GPIOHS9, GPIO.OUT)


check = 0
save = 0
#看门狗回调函数
def on_wdt(self):
    return

def on_timer(timer):  #回调函数
    global check
    global save
    data = []
    #data = com.read(2)
    if data!=None:
        if data == b'A':
            check = 1#代表存储人脸特征
        elif data == b'B':
            check = 1
            save = 1  #存到SD卡中
        elif data == b'C':
            save = 2  #清除人脸
#定时器中断初始化
tim = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_ONE_SHOT, period=500,
             unit=Timer.UNIT_MS,callback=on_timer, arg=on_timer,start=False)

#从SD卡中加载模型
task_fd = kpu.load("/sd/FaceDetection.smodel") # 加载人脸检测模型
task_ld = kpu.load("/sd/FaceLandmarkDetection.smodel") # 加载人脸五点关键点检测模型
task_fe = kpu.load("/sd/FeatureExtraction.smodel") # 加载人脸196维特征值模型


clock = time.clock()  # 初始化系统时钟，计算帧率


key_pin=16 # 设置按键引脚 FPIO16
fpioa = FPIOA()
fpioa.set_function(key_pin,FPIOA.GPIO7)
key_gpio=GPIO(GPIO.GPIO7,GPIO.IN)
last_key_state=1
key_pressed=0 # 初始化按键引脚 分配GPIO7 到 FPIO16
def check_key(): # 按键检测函数，用于在循环中检测按键是否按下，下降沿有效
    global last_key_state
    global key_pressed
    val=key_gpio.value()
    if last_key_state == 1 and val == 0:
        key_pressed=1
    else:
        key_pressed=0
    last_key_state = val

lcd.init()                          # 初始化lcd
lcd.rotation(0)
sensor.reset()                      #初始化sensor 摄像头
sensor.set_pixformat(sensor.RGB565) #设置摄像头像素
sensor.set_framesize(sensor.QVGA)   #设置窗口为配套屏幕大小
sensor.set_hmirror(0)               #设置摄像头镜像
sensor.set_vflip(2)                 #设置摄像头翻转
sensor.set_auto_gain(1,0)           #摄像头自动增益
sensor.run(1)                       #使能摄像头

#使用官方库人脸检测算法
#anchor for face detect 用于人脸检测的Anchor
anchor = (1.889, 2.5245, 2.9465, 3.94056, 3.99987, 5.3658, 5.155437, 6.92275, 6.718375, 9.01025)
#standard face key point position 标准正脸的5关键点坐标 分别为 左眼 右眼 鼻子 左嘴角 右嘴角
dst_point = [(44,59),(84,59),(64,82),(47,105),(81,105)]
#初始化人脸检测模型
a = kpu.init_yolo2(task_fd, 0.5, 0.3, 5, anchor)
img_lcd=image.Image() # 设置显示buf
img_face=image.Image(size=(128,128)) #设置 128 * 128 人脸图片buf
a=img_face.pix_to_ai() # 将图片转为kpu接受的格式
record_ftr=[] #空列表 用于存储当前196维特征
record_ftrs=[] #空列表 用于存储按键记录下人脸特征， 可以将特征以txt等文件形式保存到sd卡后，读取到此列表，即可实现人脸断电存储。
names = ['Mr.1', 'Mr.2', 'Mr.3', 'Mr.4', 'Mr.5', 'Mr.6', 'Mr.7', 'Mr.8', 'Mr.9' , 'Mr.10'] # 人名标签，与上面列表特征值一一对应。

#写入特征点到SD卡（转换为二进制）
def save_feature(feat):
    with open('/sd/data.txt','a') as f:
        record =ubinascii.b2a_base64(feat)
        f.write(record)

#清除人脸
def save_clear():
    record_ftr = []
    record_ftrs = []
    with open("/sd/data.txt","w") as f:
        f.write("")
        f.close()

#打开文件进行读取，如果有特征点信息，将其导入存储数组中
with open('/sd/data.txt','rb') as f:
    s = f.readlines()

    for line in s:
            #print(ubinascii.a2b_base64(line))
        record_ftrs.append(bytearray(ubinascii.a2b_base64(line)))
# check = 0
# save = 0
while(1): # 主循环
    #GPIO9.value(1)
    check_key() #按键检测
    tim.start()
    img = sensor.snapshot() #从摄像头获取一张图片
    clock.tick() #记录时刻，用于计算帧率
    code = kpu.run_yolo2(task_fd, img) # 运行人脸检测模型，获取人脸坐标位置

    # if save == 2:
    #     save = 0
    #     save_clear()
    #     #使用看门狗进行软件复位
    #     wdt0 = WDT(id=1, timeout=1000, callback=on_wdt, context={})
    if code: # 如果检测到人脸
        for i in code: # 迭代坐标框
            # Cut face and resize to 128x128
            a = img.draw_rectangle(i.rect()) # 在屏幕显示人脸方框
            face_cut=img.cut(i.x(),i.y(),i.w(),i.h()) # 裁剪人脸部分图片到 face_cut
            face_cut_128=face_cut.resize(128,128) # 将裁出的人脸图片 缩放到128 * 128像素
            a=face_cut_128.pix_to_ai() # 将裁出图片转换为kpu接受的格式
            #a = img.draw_image(face_cut_128, (0,0))
            # Landmark for face 5 points
            fmap = kpu.forward(task_ld, face_cut_128) # 运行人脸5点关键点检测模型
            plist=fmap[:] # 获取关键点预测结果
            le=(i.x()+int(plist[0]*i.w() - 10), i.y()+int(plist[1]*i.h())) # 计算左眼位置， 这里在w方向-10 用来补偿模型转换带来的精度损失
            re=(i.x()+int(plist[2]*i.w()), i.y()+int(plist[3]*i.h())) # 计算右眼位置
            nose=(i.x()+int(plist[4]*i.w()), i.y()+int(plist[5]*i.h())) #计算鼻子位置
            lm=(i.x()+int(plist[6]*i.w()), i.y()+int(plist[7]*i.h())) #计算左嘴角位置
            rm=(i.x()+int(plist[8]*i.w()), i.y()+int(plist[9]*i.h())) #右嘴角位置
            a = img.draw_circle(le[0], le[1], 4)
            a = img.draw_circle(re[0], re[1], 4)
            a = img.draw_circle(nose[0], nose[1], 4)
            a = img.draw_circle(lm[0], lm[1], 4)
            a = img.draw_circle(rm[0], rm[1], 4) # 在相应位置处画小圆圈
            # align face to standard position
            src_point = [le, re, nose, lm, rm] # 图片中 5 坐标的位置
            T=image.get_affine_transform(src_point, dst_point) # 根据获得的5点坐标与标准正脸坐标获取仿射变换矩阵
            a=image.warp_affine_ai(img, img_face, T) #对原始图片人脸图片进行仿射变换，变换为正脸图像
            a=img_face.ai_to_pix() # 将正脸图像转为kpu格式
            #a = img.draw_image(img_face, (128,0))
            del(face_cut_128) # 释放裁剪人脸部分图片
            # calculate face feature vector
            fmap = kpu.forward(task_fe, img_face) # 计算正脸图片的196维特征值
            feature=kpu.face_encode(fmap[:]) #获取计算结果
            reg_flag = False
            scores = [] # 存储特征比对分数
            for j in range(len(record_ftrs)): #迭代已存特征值
                score = kpu.face_compare(record_ftrs[j], feature) #计算当前人脸特征值与已存特征值的分数
                scores.append(score) #添加分数总表
            max_score = 0
            index = 0
            for k in range(len(scores)): #迭代所有比对分数，找到最大分数和索引值
                if max_score < scores[k]:
                    max_score = scores[k]
                    index = k
            if max_score > 80: # 如果最大分数大于80， 可以被认定为同一个人
                a = img.draw_string(i.x(),i.y(), ("%s :%2.1f" % (names[index], max_score)), color=(0,255,0),scale=2) # 显示人名 与 分数
                #串口发送数据，用于控制舵机
                com.write("1")
                #拉低GPIO9
                #GPIO9.value(0)  
                #屏幕显示文字识别成功
                a = img.draw_string(10,50, ("successly identify"), color=(0,255,0),scale=3)
                #刷新屏幕
                a = lcd.display(img)
                #延迟1s
                time.sleep(1)
                #拉高GPIO9
                #GPIO9.value(1)
            else:
                a = img.draw_string(i.x(),i.y(), ("X :%2.1f" % (max_score)), color=(255,0,0),scale=2) #显示未知 与 分数
            if key_pressed == 1:
                key_pressed = 0
                record_ftr = feature
                record_ftrs.append(record_ftr)  #将特征点添加到比较数组中
                save_feature(record_ftr)        #存到SD卡
            break
    a = lcd.display(img)    #刷屏显示
    #kpu.memtest()

#a = kpu.deinit(task_fe)
#a = kpu.deinit(task_ld)
#a = kpu.deinit(task_fd)

