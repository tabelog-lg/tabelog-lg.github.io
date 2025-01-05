```python
from os import environ
environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'

import pygame
import sys
import random

# 初始化
pygame.init()
screen = pygame.display.set_mode((900, 700))
clock = pygame.time.Clock()
FPS = 60


# 加载图片
bg = pygame.image.load('pic/bg.png') # 加载背景图片

left = pygame.image.load('pic/skier/skier_left.png') # 加载滑雪运动员向左的造型
right = pygame.image.load('pic/skier/skier_right.png') # 加载滑雪运动员向右的造型
end = pygame.image.load('pic/end.png')

# 加载石头障碍物
stone = pygame.image.load('pic/stone1.png')

# 加载星星
star = pygame.image.load('pic/star.png')

# 加载道具

dj1 = pygame.image.load('pic/dj1.png')
dj2 = pygame.image.load('pic/dj2.png')
dj3 = pygame.image.load('pic/dj3.png')
cloud = pygame.image.load('pic/cloud.png')
props = [dj1, dj2, dj3]


# 金币帧动画
coins1 = pygame.image.load('pic/coins/coins1.png')
coins2 = pygame.image.load('pic/coins/coins2.png')
coins3 = pygame.image.load('pic/coins/coins3.png')
coins4 = pygame.image.load('pic/coins/coins4.png')
coins = [coins1, coins2, coins3, coins4]



# 眩晕效果
eff1 = pygame.image.load('pic/eff1.png')
eff2 = pygame.image.load('pic/eff2.png')
eff3 = pygame.image.load('pic/eff3.png')
eff4 = pygame.image.load('pic/eff4.png')

# 打破得分图片
win = pygame.image.load('pic/win.png')

# 未打破得分图片
defeat = pygame.image.load('pic/defeat.png')

# 加载得分图片
num0 = pygame.image.load('pic/nums/NO0.png')
num1 = pygame.image.load('pic/nums/NO1.png')
num2 = pygame.image.load('pic/nums/NO2.png')
num3 = pygame.image.load('pic/nums/NO3.png')
num4 = pygame.image.load('pic/nums/NO4.png')
num5 = pygame.image.load('pic/nums/NO5.png')
num6 = pygame.image.load('pic/nums/NO6.png')
num7 = pygame.image.load('pic/nums/NO7.png')
num8 = pygame.image.load('pic/nums/NO8.png')
num9 = pygame.image.load('pic/nums/NO9.png')
score_dict = {"0":num0, "1":num1, "2":num2, "3":num3, "4":num4,
              "5":num5, "6":num6, "7":num7, "8":num8, "9":num9}

# 倒计时数字
n0 = pygame.image.load('pic/countdown/n0.png')
n1 = pygame.image.load('pic/countdown/n1.png')
n2 = pygame.image.load('pic/countdown/n2.png')
n3 = pygame.image.load('pic/countdown/n3.png')
n4 = pygame.image.load('pic/countdown/n4.png')
n5 = pygame.image.load('pic/countdown/n5.png')

nums = [n0, n1, n2, n3, n4, n5]


# 加载音乐文件
hit_tree = pygame.mixer.Sound('music/hit_tree.mp3')
hit_coin = pygame.mixer.Sound('music/hit_coin.mp3')
hit_award = pygame.mixer.Sound('music/hit_award.mp3')
hit_stone = pygame.mixer.Sound('music/hit_stone.mp3')
hit_prop = pygame.mixer.Sound('music/hit_prop.mp3')
pygame.mixer.music.load('music/bgm.mp3')
pygame.mixer.music.set_volume(0.5)
pygame.mixer.music.play(-1)

# 自定义事件

count = 5 # 倒计时 5 s 开始游戏
myevent = pygame.USEREVENT + 1
pygame.time.set_timer(myevent, 1000)


# 背景图片的纵坐标
y1 = 0
y2 = 700


# 滑雪运动员的矩形区域
skier_rect = left.get_rect()  # 获取滑雪者的矩形区域
skier_rect.x = 450 # 设置矩形区域的 x 坐标
skier_rect.y = 200 # 设置矩形区域的 y 坐标


# 滑雪运动员初始状态
status = 'L'

game = 'play'

game_speed = 1


# 创建树类
class Tree:
    def __init__(self):
        self.image = pygame.image.load('pic/tree.png')
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(0, 850)
        self.rect.y = random.randint(300, 600)

    def draw(self):
        screen.blit(self.image, self.rect)

    def move(self):
        self.rect.y -= game_speed
        if self.rect.y <= 0:
            self.rect.x = random.randint(0, 850)
            self.rect.y = random.randint(700, 850)    
 
    def collide(self, other):
        global game, score
        if other.colliderect(self.rect):
            if prop_status in [0, 1, 3]:
                hit_tree.play()
                game = 'over'
            elif prop_status == 2:
                hit_coin.play()
                score += 5
                self.rect.x = random.randint(0, 850)
                self.rect.y = random.randint(700, 850)

            
# 创建树的对象
tree_list = []
for i in range(3):
    tree_list.append(Tree())



# 创建金币类

class Gold:
    def __init__(self):
        self.image = pygame.image.load('pic/gold.png')
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(0, 860)
        self.rect.y = random.randint(700, 900)
    # 绘制的方法
    def draw(self):
        screen.blit(self.image, self.rect)
    # 移动的方法
    def move(self):
        self.rect.y -= game_speed
        if self.rect.y <= 0:
            self.rect.x = random.randint(0, 860)
            self.rect.y = random.randint(700, 900)
    # 碰撞检测的方法
    def collide(self, other):
        global score
        if other.colliderect(self.rect):
            hit_coin.play()
            score += 10
            self.rect.x = random.randint(0, 860)
            self.rect.y = random.randint(700, 900)

# 创建金币对象
gold_list = []
for i in range(3):
    gold_list.append(Gold())



# 创建奖励类
class Award:
    def __init__(self):
        self.coins = coins
        self.image = coins1
        self.tag = 1
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(50, 800)
        self.rect.y = random.randint(900, 1200)
    def draw(self):
        if self.tag <= 10:
             screen.blit(self.coins[0], self.rect)
        elif self.tag <= 20:
            screen.blit(self.coins[1], self.rect)
        elif self.tag <= 30:
            screen.blit(self.coins[2], self.rect)
        elif self.tag <= 40:
            screen.blit(self.coins[3], self.rect)
        self.tag += 1
        if self.tag == 41:
            self.tag = 1
    def move(self):
        self.rect.y -= game_speed
        if self.rect.y <= 0:
            self.rect.x = random.randint(50, 800)
            self.rect.y = random.randint(900, 1200)
    def collide(self, other):
        global score
        if other.colliderect(self.rect):
            hit_award.play()
            score += random.randint(10, 60)
            self.rect.x = random.randint(50, 800)
            self.rect.y = random.randint(900, 1200)


award = Award()


# 创建石头类
class Stone:
    def __init__(self):
        self.image = stone
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(200, 600)
        self.rect.y = random.randint(700, 900)

    def draw(self):
        screen.blit(self.image, self.rect)

    def move(self):
        self.rect.centery -= game_speed
        if self.rect.centery <= 0:
            self.rect.x = random.randint(200, 600)
            self.rect.y = random.randint(1000, 1500)

    def collide(self, other):
        global status, score, game
        if other.colliderect(self.rect):
            if prop_status in [0, 1, 3]:
                hit_stone.play()
                game = 'over'
            elif prop_status == 2:
                score += 3
                self.rect.x = random.randint(200, 600)
                self.rect.y = random.randint(1000, 1500)



stone_list = []
for i in range(3):
    stone_list.append(Stone())

# 创建道具
prop_num = [1, 2, 3]
prop_status = 0
class Prop:
    def __init__(self):
        self.image = pygame.image.load('pic/prop.png')
        self.num = random.choice(prop_num)
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(200, 600)
        self.rect.y = random.randint(1200, 1300)

    def draw(self):
        screen.blit(self.image, self)

    def move(self):
        self.rect.y -= game_speed
        if self.rect.y <= 0:
            self.rect.x = random.randint(200, 600)
            self.rect.y = random.randint(1200, 1300)
    def collide(self, other):
        global prop_status
        if other.colliderect(self.rect):
            hit_prop.play()
            self.rect.x = random.randint(200, 600)
            self.rect.y = random.randint(1200, 1300)
            prop_status = self.num
            prop.num = random.choice(prop_num)


prop = Prop()


# 背景移动
def move_bg():
    global y1, y2
    y1 = y1 - 1
    if y1 <= -700:
        y1 = 700
    screen.blit(bg, (0, y1))

    y2 = y2 - 1
    if y2 <= -700:
        y2 = 700
    screen.blit(bg, (0, y2))    
    

# 造型切换
def change_shape():
    global status, speed
    if prop_status in [0, 2, 3]:
        if status == 'R':
            screen.blit(right, skier_rect)
            speed = 2
        elif status == 'L':
            screen.blit(left, skier_rect)
            speed = -2


    elif prop_status == 1:
        if status == 'R':
            screen.blit(left, skier_rect)
            speed = -2
        elif status == 'L':
            screen.blit(right, skier_rect)
            speed = 2

    skier_rect.x += speed

# 绘制得分
score = 0
def draw_score():
    global score
    score_str = str(score)
    n = len(score_str)
    x, y, z = 30, 30, 35
    for i in score_str:
        screen.blit(score_dict[i], (x, y))
        x = x + z


# 眩晕效果
tally = 1
def draw_eff():
    global tally
    if tally <= 5:
        screen.blit(eff1, (skier_rect.x - 20, skier_rect.y - 80))
    elif tally <= 10:
        screen.blit(eff2, (skier_rect.x - 20, skier_rect.y - 80))
    elif tally <= 15:
        screen.blit(eff1, (skier_rect.x - 20, skier_rect.y - 80))
    elif tally <= 20:
        screen.blit(eff2, (skier_rect.x - 20, skier_rect.y - 80))
    tally += 1
    if tally == 21:
        tally = 1
# 无敌效果
def draw_star():
    screen.blit(star, ((skier_rect.x - 10, skier_rect.y - 50)))

        
# 画道具
def draw_prop():
    global count
    screen.blit(props[prop_status - 1], (600, 50))
    screen.blit(nums[count], (800, 50))
    if prop_status == 3:
        screen.blit(cloud, (100, 100))

# 画得分
score_status = 0

def show_score():
    global score, score1, score2, score_status
    if score_status == 0:
        font = pygame.font.Font('Font/FZ.ttf', 35)
        file = open('notes.txt', 'r')
        str1 = file.readline()
        str2 = str1[1:len(str1) - 1]  # 仅保留字符
        old_list = str2.split(',')  # 对字符进行分隔
        new_list = []
        for i in old_list:  # 生成新的列表
            new_list.append(int(i))
        if max(new_list) > score:
            max_score = max(new_list)
            max_score = str(max_score)
            scores = str(score)
            score1 = font.render(max_score, True, (0, 0, 0))
            score2 = font.render(scores, True, (0, 0, 0))

            screen.blit(defeat, (280, 200))
            screen.blit(score1, (350, 350))
            screen.blit(score2, (500, 350))

        else:
            max_score = score
            new_list.append(score)
            max_score = str(max_score)
            score1 = font.render(max_score, True, (0, 0, 0))
            screen.blit(win, (260, 200))  # 绘制背景
            screen.blit(score1, (400, 350))


        file = open('notes.txt', 'w')
        file.write(str(new_list))


        file.close()
        score_status = 1


# 主循环模块
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit()
        if prop_status in [1, 2, 3]:

            if event.type == myevent:
                count -= 1
                if count == 0:
                    prop_status = 0
                    count = 5

        # 设置状态 status 的值
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_RIGHT:
                status = 'R'
            if event.key == pygame.K_LEFT:
                status = 'L'

    # 防止滑雪运动员移出边界的代码
    if skier_rect.x < 0:
        skier_rect.x = 0
    elif skier_rect.x > 840:
        skier_rect.x = 840
    
    
    if game == 'play':
        # 背景移动的代码
        move_bg()
        # 绘制得分
        draw_score()
        # 滑雪运动员切换造型的代码
        change_shape()
         # 树的绘制和移动
        for tree in tree_list:
            tree.draw()
            tree.move()
            tree.collide(skier_rect)

        # 金币的绘制和移动
   
        for coin in gold_list:
            coin.draw()
            coin.move()
            coin.collide(skier_rect)


        # 奖励
        award.draw()
        award.move()
        award.collide(skier_rect)

        # 道具
        prop.draw()
        prop.move()
        prop.collide(skier_rect)


        # 根据得分设置速度
        if score <= 50:
            game_speed = 1
        elif score <= 100:
            game_speed = 2
        elif score <= 150:
            game_speed = 3
        else:
            game_speed = 4
            for stone in stone_list:
                stone.draw()
                stone.move()
                stone.collide(skier_rect)
        # 画道具
        if prop_status != 0:
            draw_prop()
            if prop_status == 1:
                draw_eff()
            elif prop_status == 2:
                draw_star()
    elif game == 'over':
        show_score()


    pygame.display.update()
    clock.tick(FPS)

```