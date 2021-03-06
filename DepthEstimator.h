#pragma once

#include <opencv2/ocl/ocl.hpp>
#include "LightFieldPicture.h"

/**
 * The abstract base class for any algorithm which can estimate a depth map
 * from light-field data.
 *
 * @author      Kai Puth <kai.puth@student.htw-berlin.de>
 * @version     0.1
 * @since       2014-06-24
 */
class DepthEstimator
{
public:
	DepthEstimator(void);
	~DepthEstimator(void);

	virtual oclMat estimateDepth(const LightFieldPicture& lightfield) =0;

	// accessors for results
	//virtual oclMat getDepthMap() const;
	//virtual oclMat getConfidenceMap() const;
	//virtual oclMat getExtendedDepthOfFieldImage() const;

};

