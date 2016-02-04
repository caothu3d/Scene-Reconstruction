import numpy as np
import cv2
import exifread, sys
from KMatrix import KMatrix

class Image:
    def __init__(self, filepath):
        self.fname = filepath
        self.img = cv2.imread(filepath)
        self.height = self.img.shape[0]
        self.width = self.img.shape[1]

        try:
            with open(filepath, 'r') as f:
                self.tags = exifread.process_file(f)
            self.focal_length_num = float(self.tags['EXIF FocalLength'].values[0].num)
            self.focal_length_den = self.tags['EXIF FocalLength'].values[0].den
        except:
            self.tags = {}
            self.focal_length_num = 1
            self.focal_length_den = 1

        self.kps = None
        self.descs = None

        # CALCULATE K

        # First, find focal length in mm
        mm_focal_length = self.focal_length_num / self.focal_length_den
        # CCD width is the width of the image sensor (using iphone as default for now)
        ccdWidth = 4.89
        # Calculate focal length in pixels
        self.focalLength = self.width * mm_focal_length / ccdWidth
        print mm_focal_length
        center_x = float(self.width) / 2
        center_y = float(self.height) / 2

        self.K = KMatrix(focalLength=self.focalLength, principalPoint=(center_x, center_y))

    def detect_features(self):
        sift = cv2.xfeatures2d.SIFT_create()
        (self.kps, self.descs) = sift.detectAndCompute(self.img, None)

    def __repr__(self):
        if not self.kps and not self.descs: 
            return "{} - Focal Length: {} / {} - KPs: {} Descs: {}".format(
                self.fname, 
                self.focal_length_num, 
                self.focal_length_den)
        else:
            return "{} - Focal Length: {} / {} - KPs: {} Descs: {}".format(
                self.fname, 
                self.focal_length_num, 
                self.focal_length_den,
                len(self.kps),
                len(self.descs))
