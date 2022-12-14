import cv2
import numpy as np
import oneShotMatch
import sys
import subprocess
import src.python.GridCell

# fullscale3 #
# imgPath='driver/vlcsnap-2022-04-05-15h36m34s616.png'
# mc = np.matrix([[-0.18071, -0.77495, 573.08481],
#                 [0.77063, 0.06183, -34.54974],
#                 [-0.00005, -0.00003, 1.00000]])
# #
def run(filenameMatrixOrActualMatrixObject,
        filenameFirstImageOrActualMatrixObject,
        x,y,intermediateImagePath,
        mc=None # constant matrix
        ,showPreviewWindow=False):
    imgPath=intermediateImagePath
    print("intermediateImagePath:", intermediateImagePath, "mc:", mc)
    # if isinstance(filenameFirstImageOrActualMatrixObject, str):
    #     firstImage = cv2.imread(filenameFirstImageOrActualMatrixObject)
    # else:
    #     firstImage = filenameFirstImageOrActualMatrixObject
    p0=np.array([x,y])
    # Get matrix as python list
    # HACK: no quote handling for filenameMatrix below
    print("filenameMatrixOrActualMatrixObject:",filenameMatrixOrActualMatrixObject)
    if isinstance(filenameMatrixOrActualMatrixObject, str):
        filenameMatrix=filenameMatrixOrActualMatrixObject
        evalThisDotStdout=subprocess.run(['bash', '-c', "cat \"" + filenameMatrix + "\" | sed 's/^/[/' | sed -E 's/.$/]&/' | sed -E 's/;/,/'"], capture_output=True, text=True)
        stdout_=evalThisDotStdout.stdout
        print("about to eval (stderr was", evalThisDotStdout.stderr, "):", stdout_)
        m0 = np.matrix(eval(stdout_))
    else:
        m0=filenameMatrixOrActualMatrixObject
    print("m0:",m0)
    if showPreviewWindow:
        if isinstance(filenameFirstImageOrActualMatrixObject, str):
            firstImage = cv2.imread(filenameFirstImageOrActualMatrixObject)
        else:
            firstImage = filenameFirstImageOrActualMatrixObject
        
        # Show landing position
        import knn_matcher2
        knn_matcher2.showPreviewWindow = True
        knn_matcher2.waitAmountStandard = 0
        knn_matcher2.showLandingPos(firstImage, m0)
    try:
        m1, firstImageWidth, firstImageHeight, firstImage_, firstImageOrig, firstImageFilename_ = oneShotMatch.run(filenameFirstImageOrActualMatrixObject, imgPath, showPreviewWindow)
    except:
        print("satellite matcher handler exception, will use id matrix:")
        import traceback
        traceback.print_exc()
        
        if isinstance(filenameFirstImageOrActualMatrixObject, str):
            firstImage = cv2.imread(filenameFirstImageOrActualMatrixObject)
        else:
            firstImage = filenameFirstImageOrActualMatrixObject
        m1=np.matrix([[1.0, 0.0, 0.0],
                      [0.0, 1.0, 0.0],
                      [0.0, 0.0, 1.0]])
        # hOrig, wOrig = firstImage.shape[:2]
        # firstImageWidth = wOrig
        # firstImageHeight = hOrig
        firstImage_ = firstImage
        firstImageOrig = firstImage
        firstImageFilename_ = filenameFirstImageOrActualMatrixObject if isinstance(filenameFirstImageOrActualMatrixObject, str) else None
    print("m1:",m1)
    # Reference for the below: pPrime = p0*np.linalg.pinv(m0)*m1*mc
    pPrime = cv2.perspectiveTransform(cv2.perspectiveTransform(cv2.perspectiveTransform(np.array([[p0]], dtype=np.float32), np.linalg.pinv(m0)),m1),mc)
    print("pPrime:", pPrime)
    pPrime=pPrime[0][0] # unwrap the junk one-element embedding arrays
    return src.python.GridCell.getGridCellIdentifier(1796, 1796, pPrime[0], pPrime[1]), m0, m1, mc, firstImage_, firstImageOrig, firstImageFilename_
