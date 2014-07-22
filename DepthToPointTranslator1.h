#pragma once
#include "depthtopointtranslator.h"

/**
 * The abstract base class for any algorithm that translates reconstructed depth
 * into 3D points.
 *
 * @author      Kai Puth <kai.puth@student.htw-berlin.de>
 * @version     0.1
 * @since       2014-07-22
 */
class DepthToPointTranslator1 :
	public DepthToPointTranslator
{
	Mat makeCalibrationMatrix(const LightFieldPicture& lightfield,
		const Size& imageSize) const;

public:
	DepthToPointTranslator1(void);
	~DepthToPointTranslator1(void);

	oclMat translateDepthToPoints(const oclMat& depth,
		const LightFieldPicture& lightfield) const;
};
