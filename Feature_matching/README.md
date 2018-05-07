# Sequence of techniques used for template matching:

### SURF
This is used for **keypoint extraction**
**_minHessian_** has to be used for setting the threshold for **keypoint** selection.

### FLANN Matcher
This is used for finding the **best matches**. A bounding box is drawn on the **Good Matches**.
Inorder to avoid the crash of software when good match is not found

### Homography Transform
Homography transformation is used to find the transformation between **Two planes** i.e. **_Image Plane_** 
and the **_Sample Data_** frame.
