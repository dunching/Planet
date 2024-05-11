// Copyright Ben Sutherland 2023. All rights reserved.

// Modified from:
//https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt

/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int TriBoxOverlap(const float* BoxCenter,  */
/*          const float* BoxHalfSize, const FVector& V0,*/
/*          const FVector& V1, const FVector& V2);      */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/

/*
Copyright 2020 Tomas Akenine-Möller

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "CoreMinimal.h"
#include "FlyingNavSystemTypes.h"

#define X_i 0
#define Y_i 1
#define Z_i 2

#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;


inline int PlaneBoxOverlap(const FCoord normal[3], const FCoord vert[3], const FCoord maxbox[3])	// -NJMP-
{
  int q;
  FCoord vmin[3],vmax[3],v;
  for(q=X_i;q<=Z_i;q++)
  {
    v=vert[q];					// -NJMP-
    if(normal[q]>0.0f)
    {
      vmin[q]=-maxbox[q] - v;	// -NJMP-
      vmax[q]= maxbox[q] - v;	// -NJMP-
    }
    else
    {
      vmin[q]= maxbox[q] - v;	// -NJMP-
      vmax[q]=-maxbox[q] - v;	// -NJMP-
    }
  }
  if(DOT(normal,vmin)>0.0f) return 0;	// -NJMP-
  if(DOT(normal,vmax)>=0.0f) return 1;	// -NJMP-

  return 0;
}


/*======================== X-tests ========================*/

#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v0[Y_i] - b*v0[Z_i];			       	   \
	p2 = a*v2[Y_i] - b*v2[Z_i];			       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * BoxHalfSize[Y_i] + fb * BoxHalfSize[Z_i];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0[Y_i] - b*v0[Z_i];			           \
	p1 = a*v1[Y_i] - b*v1[Z_i];			       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * BoxHalfSize[Y_i] + fb * BoxHalfSize[Z_i];   \
	if(min>rad || max<-rad) return 0;


/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0[X_i] + b*v0[Z_i];		      	   \
	p2 = -a*v2[X_i] + b*v2[Z_i];	       	       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * BoxHalfSize[X_i] + fb * BoxHalfSize[Z_i];   \
	if(min>rad || max<-rad) return 0;


#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0[X_i] + b*v0[Z_i];		      	   \
	p1 = -a*v1[X_i] + b*v1[Z_i];	     	       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * BoxHalfSize[X_i] + fb * BoxHalfSize[Z_i];   \
	if(min>rad || max<-rad) return 0;



/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1[X_i] - b*v1[Y_i];			           \
	p2 = a*v2[X_i] - b*v2[Y_i];			       	   \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * BoxHalfSize[X_i] + fb * BoxHalfSize[Y_i];   \
	if(min>rad || max<-rad) return 0;



#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0[X_i] - b*v0[Y_i];				   \
	p1 = a*v1[X_i] - b*v1[Y_i];			           \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * BoxHalfSize[X_i] + fb * BoxHalfSize[Y_i];   \
	if(min>rad || max<-rad) return 0;


inline int triBoxOverlap(const FCoord* BoxCenter, const FCoord* BoxHalfSize, const FVector& V0, const FVector& V1, const FVector& V2)
{
  /*    use separating axis theorem to test overlap between triangle and box */
  /*    need to test for overlap in these directions: */
  /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
  /*       we do not even need to test these) */
  /*    2) normal of the triangle */
  /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
  /*       this gives 3x3=9 more tests */
	
   FCoord v0[3],v1[3],v2[3];
   //float axis[3];
   FCoord min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
   FCoord normal[3],e0[3],e1[3],e2[3];

   /* This is the fastest branch on Sun */
   /* move everything so that the boxcenter is in (0,0,0) */
   SUB(v0,V0,BoxCenter);
   SUB(v1,V1,BoxCenter);
   SUB(v2,V2,BoxCenter);

   /* compute triangle edges */
   SUB(e0,v1,v0);      /* tri edge 0 */
   SUB(e1,v2,v1);      /* tri edge 1 */
   SUB(e2,v0,v2);      /* tri edge 2 */


   /* Bullet 3:  */
   /*  test the 9 tests first (this was faster) */
   fex = FMath::Abs(e0[X_i]);
   fey = FMath::Abs(e0[Y_i]);
   fez = FMath::Abs(e0[Z_i]);
   AXISTEST_X01(e0[Z_i], e0[Y_i], fez, fey);
   AXISTEST_Y02(e0[Z_i], e0[X_i], fez, fex);
   AXISTEST_Z12(e0[Y_i], e0[X_i], fey, fex);

   fex = FMath::Abs(e1[X_i]);
   fey = FMath::Abs(e1[Y_i]);
   fez = FMath::Abs(e1[Z_i]);

   AXISTEST_X01(e1[Z_i], e1[Y_i], fez, fey);
   AXISTEST_Y02(e1[Z_i], e1[X_i], fez, fex);
   AXISTEST_Z0(e1[Y_i], e1[X_i], fey, fex);

   fex = FMath::Abs(e2[X_i]);
   fey = FMath::Abs(e2[Y_i]);
   fez = FMath::Abs(e2[Z_i]);

   AXISTEST_X2(e2[Z_i], e2[Y_i], fez, fey);
   AXISTEST_Y1(e2[Z_i], e2[X_i], fez, fex);
   AXISTEST_Z12(e2[Y_i], e2[X_i], fey, fex);

   /* Bullet 1: */
   /*  first test overlap in the {x,y,z}-directions */
   /*  find min, max of the triangle each direction, and test for overlap in */
   /*  that direction -- this is equivalent to testing a minimal AABB around */
   /*  the triangle against the AABB */

   /* test in X-direction */
   FINDMINMAX(v0[X_i],v1[X_i],v2[X_i],min,max);
   if(min>BoxHalfSize[X_i] || max<-BoxHalfSize[X_i]) return 0;

   /* test in Y-direction */
   FINDMINMAX(v0[Y_i],v1[Y_i],v2[Y_i],min,max);
   if(min>BoxHalfSize[Y_i] || max<-BoxHalfSize[Y_i]) return 0;

   /* test in Z-direction */
   FINDMINMAX(v0[Z_i],v1[Z_i],v2[Z_i],min,max);
   if(min>BoxHalfSize[Z_i] || max<-BoxHalfSize[Z_i]) return 0;

   /* Bullet 2: */
   /*  test if the box intersects the plane of the triangle */
   /*  compute plane equation of triangle: normal*x+d=0 */
   CROSS(normal,e0,e1);

   // -NJMP- (line removed here)
   if(!PlaneBoxOverlap(normal,v0,BoxHalfSize)) return 0;	// -NJMP-

   return 1;   /* box and triangle overlaps */
}

//----------------------------------------------------------------------//
// UE4 convenience overloads
//----------------------------------------------------------------------//

// Raw vert array version
inline bool UETriBoxOverlap(const FVector& BoxCentre, const FVector& BoxExtent, const FVector& V0, const FVector& V1, const FVector& V2)
{
	return (bool)triBoxOverlap((FCoord*)&BoxCentre, (FCoord*)&BoxExtent, V0, V1, V2);
}

// Copy straight from triangle array
inline bool UETriBoxOverlap(const FVector& BoxCentre, const FVector& BoxExtent, const int32* Indices, const FCoord* Coords, const int32 TriNum)
{
	return UETriBoxOverlap(BoxCentre, BoxExtent,
		FlyingNavSystem::CoordToVec(Coords, Indices[3*TriNum + 0]),
		FlyingNavSystem::CoordToVec(Coords, Indices[3*TriNum + 1]),
		FlyingNavSystem::CoordToVec(Coords, Indices[3*TriNum + 2])
		);
}
// Copy straight from triangle array
inline bool UETriBoxOverlap(const FVector& BoxCentre, const FVector& BoxExtent, const TArray<int32>& Indices, const TArray<FCoord>& Coords, const int32 TriNum)
{
	return UETriBoxOverlap(BoxCentre, BoxExtent, Indices.GetData(), Coords.GetData(), TriNum);
}

// From triangle array, with transform (bit slow though)
inline bool UETriBoxOverlap(const FVector& BoxCentre, const FVector& BoxExtent, const int32* Indices, const FCoord* Coords, const int32 TriNum, const FTransform& Transform)
{
	const FVector V1 = Transform.TransformPosition(*(FVector*)&Coords[3*Indices[3*TriNum + 0]]);
	const FVector V2 = Transform.TransformPosition(*(FVector*)&Coords[3*Indices[3*TriNum + 1]]);
	const FVector V3 = Transform.TransformPosition(*(FVector*)&Coords[3*Indices[3*TriNum + 2]]);

	return UETriBoxOverlap(BoxCentre, BoxExtent, V1, V2, V3);
}