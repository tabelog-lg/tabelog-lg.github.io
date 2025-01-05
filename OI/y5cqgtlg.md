反常积分
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}\mathrm{d}x $$

转换
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}e^{-ax}\mathrm{d}x $$

改为函数
$$ I(a) = \int_{0}^{\infty}\frac{\sin(x)}{x}e^{-ax}\mathrm{d}x \tag{1}$$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{\mathrm{d}}{\mathrm{d}a}(\int_{0}^{\infty}\frac{\sin(x)}{x}e^{-ax}\mathrm{d}x) $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \int_{0}^{\infty}\frac{\mathrm{d}}{\mathrm{d}a}(\frac{\sin(ax)}{x}e^{-ax}\mathrm{d}x) $$

化简
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \int_{0}^{\infty}\frac{\sin(x)}{x}(-x)e^{-ax}\mathrm{d}x $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \int_{0}^{\infty}\sin(x)(-e^{-ax})\mathrm{d}x $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = -\int_{0}^{\infty}\sin(x)e^{-ax}\mathrm{d}x $$

展开
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = + \frac{\sin(x)e^{-ax}}{a}\bigg|_{0}^{\infty} + \int_{0}^{\infty}-\frac{\cos(x)}{a}e^{-ax}x\mathrm{d}x $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = 0 - \int_{0}^{\infty}\frac{\cos(x)e^{-ax}}{a}x\mathrm{d}x $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{\cos(x)e^{-ax}}{a^2}\bigg|_{0}^{\infty} - \frac{1}{a^2}\int_{0}^{\infty}\sin(x)e^{-ax}x\mathrm{d}x $$

化简
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2} - \frac{\mathrm{d}}{\mathrm{d}a}I(a) $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) + \frac{1}{a^2}\frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2} $$
$$ (1 + \frac{1}{a^2})\frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2} $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2}: (1 + \frac{1}{a^2})$$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2}: (\frac{a^2 + 1}{a^2}) $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2} \times (\frac{a^2}{a^2 + 1}) $$
$$ \frac{\mathrm{d}}{\mathrm{d}a}I(a) = \frac{-1}{a^2 + 1} $$
$$ \int\frac{\mathrm{d}}{\mathrm{d}a}I(a)\mathrm{d}a = \int\frac{-1}{a^2 + 1}\mathrm{d}a $$
$$ I(a) = - \arctan(a) + C \tag{2} $$

将 (1) 式代入 (2) 式
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}e^{-ax}\mathrm{d}x = - \arctan(a) + C \tag{3}$$

转换 (2) 式
$$ I(\infty) = \int_{0}^{\infty}\frac{\sin(x)}{x}e^{-\infty}\mathrm{d}x $$
$$ I(\infty) = \int_{0}^{\infty}0\mathrm{d}x $$
$$ I(\infty) = 0 \tag{4}$$

将 (2) 式代入 (4) 式
$$ I(\infty) = 0 = - \arctan(\infty) + C $$
$$ I(\infty) = 0 = - \arctan(\infty) + C = -\frac{\pi}{2} + C $$
$$ I(a) = 0 = - \arctan(a) + \frac{\pi}{2} \tag{5}$$

将 (5) 式代入 (3) 式
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}e^{-ax}\mathrm{d}x = - \arctan(a) + \frac{\pi}{2} $$
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}e^{-0 \times x}\mathrm{d}x = - \arctan(0) + \frac{\pi}{2} $$
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}\mathrm{d}x = - \arctan(0) + \frac{\pi}{2} $$
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}\mathrm{d}x = - 0 + \frac{\pi}{2} $$
$$ \int_{0}^{\infty}\frac{\sin(x)}{x}\mathrm{d}x = \frac{\pi}{2} $$