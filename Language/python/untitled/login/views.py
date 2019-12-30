from django.shortcuts import render
from django.shortcuts import redirect
from login import forms

from django.db import models

# Create your views here.
def index(request):
    pass
    return render(request, 'login/index.html')


# def login(request):
#     if request.method == 'POST':
#         username = request.POST.get('username', None)  #如果没有username字段，则会赋值默认值
#         password = request.POST.get('password')
#         message = "所有字段都必须填写！"
#         if username and password:  # 确保用户名和密码都不为空
#             username = username.strip()
#             password = password.strip()
#             try:
#                 if "hao" == password:
#                     return redirect('/index/')#将页面重定向到主页
#                 else:
#                     message = "密码不正确！"
#             except:
#                 message = "用户名不存在！"
#         return render(request, 'login/login.html',{"message":message})
#     return render(request, 'login/login.html')

def login(request):
    if request.method == "POST":
        login_form = forms.UserForm(request.POST)
        message = "请检查填写的内容！"
        if login_form.is_valid():
            username = login_form.cleaned_data['username']
            password = login_form.cleaned_data['password']
            try:
                if "hao" == password:
                    return redirect('/index/')#将页面重定向到主页
                else:
                    message = "密码不正确！"
            except:
                message = "用户名不存在！"
        return render(request, 'login/login.html', locals())

    login_form = forms.UserForm()
    return render(request, 'login/login.html', locals())

def register(request):
    pass
    return render(request, 'login/register.html')


def logout(request):
    pass
    return redirect("/index/")