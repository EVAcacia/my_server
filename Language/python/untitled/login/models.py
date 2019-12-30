from django.db import models

# Create your models here.
class User(models.Model):

    gender = (
        ('male', "男"),
        ('female', "女"),
    )

    name = models.CharField(max_length=128, unique=True)  #name必填，最长不超过128个字符，并且唯一，也就是不能有相同姓名；
    password = models.CharField(max_length=256)      #password必填，最长不超过256个字符（实际可能不需要这么长）；
    email = models.EmailField(unique=True)      #email使用Django内置的邮箱类型，并且唯一；
    sex = models.CharField(max_length=32, choices=gender, default="男") #性别使用了一个choice，只能选择男或者女，默认为男；
    c_time = models.DateTimeField(auto_now_add=True) #使用__str__帮助人性化显示对象信息；

    def __str__(self):
        return self.name

    class Meta:
        ordering = ["-c_time"]
        verbose_name = "用户"
        verbose_name_plural = "用户"