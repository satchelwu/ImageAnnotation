# ImageAnnotation
ImageAnnotation Tool makes tagging object easier, faster and more effective. Hope it can help you a lot. Email:thin.wu@163.com

Windows:
Exe:链接：http://pan.baidu.com/s/1kU8tWiF 密码：thgr

GUI
![image](https://github.com/satchelwu/ImageAnnotation/blob/master/1.jpg)
![image](https://github.com/satchelwu/ImageAnnotation/blob/master/2.jpg)

Usage: 
“OpenDir”  shortcut = Ctrl + O 
Open the Directory which contains images(.jpg .bmp .png). 
  
“ChangeSaveDir”  shortcut = Ctrl + P 
Change the Directory which contains the annotation files(.xml). 
  
“Prev Image” shortcut = ← or ↑ 
Load the previous image from the current selected image path in image path list   
  
“Next Image” shortcut = → or ↓ 
Load the next image from the current selected image path in image path list 
  
“Create” shortcut = W 
Create a bounding box with tag on the current showing image. If “Create” is Clicked or shortcut(W) is pressed, then you can draw a bounding box. When drawing completed(left button up), a select label window will pop up, then you can double clicked an existed label(tag). Here is a trick. If “Use Default Label” is Checked and the right textbox is not empty, the select label window will not pop up and the string in the right textbox will be used as the default object label(tag). 

“Export” 
Export the labeled object into separate folders whose name is the same as label(tag) 

Add new categories
You can add the new categories by modifying the labels.txt and Restart.

Some other skills: 
You can move and resize the current showing image by mouse left button and mouse wheel. The thumbnail image on the bottom left can be double clicked, it can help you locate the position fast. 
  
References: 
labelImg  https://github.com/tzutalin/labelImg 
  
Thanks: 
@燕山大学～硕士～Bless  First user of the tool, giving a lot of advice. Thanks very much. 

