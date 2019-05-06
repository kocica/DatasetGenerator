﻿# Dataset generator

This simple repository provides implementation of full-image dataset generator. Resulting synthetic dataset is supposed to be used for CNN (primary single-shot detector) training.

## Dependencies

 - OpenCV 3.x and higher

## About the tool

Tool works on simple principle of placing objects to the background images on the random positions and can work in two slightly different ways, depending on what type of data is available for your use-case.

### 1) Using cropped objects
Cropped objects (used for classifier training) are placed to the images of background using Poisson blending [1].

<b>Cropped traffic sign placed to the background with (right) and without (left) Poisson blending</b>:<br>
<p align="center">
  <img src="data/readme_imgs/cropped_1.png" alt="Cropped without poisson blending" width="204" height="155">
  <img src="data/readme_imgs/cropped_2.png" alt="Cropped with poisson blending" width="204" height="155">
</p>

### 2) Using transparent objects

Before transparent objects are placed into background, few effects are applied to synthetically modify appearance of the object. Whether or not are the effects applied is based on chosen probability (reflecting real-time conditions).

List of applied effects:

 - Rotation in X, Y, Z axis
 - Brightness modification (according to the brightness of the background)
 - Gamma correction
 - Blur
 - Noise (Salt and papper, Gaussian)
 - ROI selection
 - Resize of the objects
 - Hue modification
 - Brightness gradient
 - Object overlapping
 - Flipping (but be aware: some objscts may have different meaning, e.g. left and right arrow, hand, etc.)

<b>Visual representation of most effects applied on the traffic sign separatelly (original image in the top left corner)</b>:<br>
<p align="center">
  <img src="data/readme_imgs/effects.png" alt="Effects">
</p>

<b>Then objects are placed into background</b>:<br>
<p align="center">
  <img src="data/readme_imgs/synt_1.png" alt="Transparent" width="204" height="155">
  <img src="data/readme_imgs/synt_2.jpg" alt="Transparent" width="204" height="155">
</p>

In general, second approach has better results. Combination of both approaches does not work better. By far the best results have been achieved using combination of many synthetic images in combination with few real labeled images, which keeps training in the right direction.

## Annotations

This tool is using YOLO [2] annotation format:

```
<object_class> <x> <y> <width> <height>
```

Both, object oordinates and size are relative - values in the interval (0,1) - to the full image size.

## Usage

 - Place images of the backgrounds (where objects should be placed - in case of the traffic signs, images should originate from the city environment, but cannot contain any of the generated objects - they would miss the annotation and so negativelly bias the training process) to the `data/backgrounds` folder
 - In case of using transparent objects, place them to the `data/transparent` folder, otherwise to the `data/cropped` folder. Place files of each class to separate folders and save unique class name to the `imgClass` file located in the same folder
 - In the Makefile, select (comment/uncomment) effects you want to be applied
 - Type `make` in the root folder
 - In case of using transparent objects, type `make runt`, otherwise `make runc`
 - Generated dataset should be in the `out` folder

## Results

Video evaluated using model trained on the traffic sign synthetic dataset generated by this tool:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=J9hYBg76nNQ" target="_blank"><img src="http://img.youtube.com/vi/J9hYBg76nNQ/0.jpg"
alt="Traffic sign detection" width="240" height="180" border="10" /></a>

## Future work

 - Load both - selected effects and applied value ranges from the configuration file.
 - For more realistic synthetic dataset, perform 3D analyzation of the both background image and object to be placed to the background.
 - Further research on synthetic dataset CNN training (i.e. what does bias the training, what improves the overall performance, etc.)

## References

[1] [Martino, Matías Di, Gabriele Facciolo and Enric Meinhardt. <em>"Poisson Image Editing."</em> IPOL Journal 6 (2016): 300-325.](https://dl.acm.org/citation.cfm?id=882269)

[2] [Joseph Redmon, Santosh Divvala, Ross Girshick, Ali Farhadi. <em>"You Only Look Once: Unified, Real-Time Object Detection."</em> CoRR Journal (2015)](https://arxiv.org/abs/1506.02640)
