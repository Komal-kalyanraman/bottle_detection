# Sequence of techniques used for template matching:

### SURF
This is used for **keypoint extraction**. For setting the threshold for **keypoint** selection **_minHessian_** has to be adjusted.

### FLANN Matcher
This is used for finding the **best matches**. A bounding box is drawn on the **Good Matches**.
Inorder to avoid the crash of software when good match is not found

### Homography Transform
Homography transformation is used to find the transformation between **Two planes** i.e. **_Image Plane_** 
and the **_Sample Data_** frame.

Code performance sample video : https://www.youtube.com/watch?v=nTCCCWmLQ6U&feature=youtu.be
