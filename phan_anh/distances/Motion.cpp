#include <cstdio>
#include <cmath>

using namespace std;

#include "util.h"
#include "Motion.h"
#include "Tippunkt.h"
#include "Werkzeug.h"
#include "Tippunkt.h"
#include "Konstanten.h"


// Abstand des Werkzeugs bei Bewegung zum Zeitpunkt t
double SweepDist::Motion::dist_statisch
( double t, 
  Werkzeug &wz, 
  double point_w[],
  double grad_w[],
  double wnp_w[],
  double *abl
)
{ 
  Tippunkt tp;
  double dist;

  mach_tippunkt(t, tp);
  double point_wz[3];
  double grad_wz[3];
  double wnp_wz[3];
  double speed_v[3];
  double geschw;

  tp.punkt_welt_tippunkt(point_w, point_wz);

  dist = wz.dist_point(point_wz, grad_wz, wnp_wz);
  tp.vektor_tippunkt_welt(grad_wz, grad_w);
  tp.punkt_tippunkt_welt(wnp_wz, wnp_w);
  geschw = speed(t,wnp_wz,speed_v);
 
  *abl = - skalarprodukt3(grad_w, speed_v);
  
  return dist;
}

// Abstand der Werkzeugscheibe bei Bewegung zum Zeitpunkt t
double SweepDist::Motion::dist_statisch
( double t, 
  Werkzeugscheibe &twz, 
  double point_w[],
  double grad_w[],
  double wnp_w[],
  double *abl
)
{
  Tippunkt tp;
  double dist;

  mach_tippunkt(t,tp);

  double point_wz[3];
  double grad_wz[3];
  double wnp_wz[3];
  double speed_v[3];
  double geschw;

  tp.punkt_welt_tippunkt(point_w, point_wz);

  dist = twz.dist_point(point_wz, grad_wz, wnp_wz);
  tp.vektor_tippunkt_welt(grad_wz, grad_w);
  tp.punkt_tippunkt_welt(wnp_wz, wnp_w);
  geschw = speed(t,wnp_wz,speed_v);
  *abl = - skalarprodukt3(grad_w, speed_v);

  return dist;
}

int SweepDist::Motion::min_dist
( Werkzeug &werkzeug,
  double point_wrld[],
  double *t_min,
  double *dist_min,
  double grad_wrld[],

  double t0,
  double t1
)
{
#ifdef MOTION
  fprintf(stderr,"Motion: min_dist(Werkzeug)\n");
#endif

  Tippunkt tp;
  double t2, dist0, dist1, dist2;
  double abl0, abl1, abl2;
  double grad0_wrld[3], grad1_wrld[3], grad2_wrld[3];

  double point_tool[3];
  double wnp_tool[3];
  double grad_tool[3];
  double geschw;
  double geschw_v[3];

  mach_tippunkt(t0, tp);
  tp.punkt_welt_tippunkt(point_wrld, point_tool);

  dist0 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
  if ( dist0 <= 0)
    return -1;
  tp.vektor_tippunkt_welt(grad_tool, grad0_wrld);
  geschw = speed(t0, wnp_tool, geschw_v);
  abl0 = -skalarprodukt3(grad0_wrld, geschw_v);

  if ( fabs(abl0) < EPS_FKT)
  { *t_min = t0;
    *dist_min = dist0;
    for ( int i = 0; i < 3; ++i)
      grad_wrld[i] = grad0_wrld[i];

    return 0;
  }

  if ( abl0 > 0)
  { *t_min = t0;
    *dist_min = dist0;
    for ( int i = 0; i < 3; ++i)
      grad_wrld[i] = grad0_wrld[i];

    return 1;
  }
  mach_tippunkt(t1, tp);
  tp.punkt_welt_tippunkt(point_wrld, point_tool);
  dist1 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
  if ( dist1 <= 0)
    return -1;
  tp.vektor_tippunkt_welt(grad_tool, grad1_wrld);
  geschw = speed(t1, wnp_tool, geschw_v);
  abl1 = -skalarprodukt3(grad1_wrld, geschw_v);

  if ( fabs(abl1) < EPS_FKT)
  { *t_min = t1;
    *dist_min = dist1;
    for ( int i = 0; i < 3; ++i)
      grad_wrld[i] = grad1_wrld[i];

    return 0;
  }
  if ( abl1 < 0)
  { *t_min = t1;
    *dist_min = dist1;
    for ( int i = 0; i < 3; ++i)
      grad_wrld[i] = grad1_wrld[i];

    return 1;
  }

#ifdef MOTION
  fprintf(stderr,"t0=%20.10e  dist0=%20.10e abl0=%20.10e\n", t0, dist0, abl0);
  fprintf(stderr,"t1=%20.10e  dist1=%20.10e abl1=%20.10e\n", t1, dist1, abl1);
  fprintf(stderr,"Pegasus\n");
  int count = 0;
#endif
  while ( fabs(t1-t0) > EPS_T)
  {
#ifdef MOTION
    ++count;
    fprintf(stderr,"Schritt=%d: ", count);
    fprintf(stderr,"t0=%20.15e dist0=%20.15e abl0=%20.15e ", t0, dist0, abl0);
    fprintf(stderr,"t1=%20.15e dist1=%20.15e abl1=%20.15e\n", t1, dist1, abl1);
#endif
    t2 = t1 - abl1 * ( t1 - t0) /(abl1 - abl0);
    mach_tippunkt(t2, tp);
    tp.punkt_welt_tippunkt(point_wrld, point_tool);
    dist2 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
    if ( dist2 <= 0)
      return -1;
    tp.vektor_tippunkt_welt(grad_tool, grad2_wrld);
    geschw = speed(t2, wnp_tool, geschw_v);
    abl2 = -skalarprodukt3(grad2_wrld, geschw_v);

    if ( fabs(abl2) < EPS_FKT)
    { *t_min = t2;
      *dist_min = dist2;
      for ( int i = 0; i < 3; ++i)
        grad_wrld[i] = grad2_wrld[i];

      return 0;
    }


    if ( abl1 * abl2 < 0)
    {
      t0 = t1;
      abl0 = abl1;
      dist0 = dist1;
      t1 = t2;
      abl1 = abl2;
      dist1 = dist2;
    }
    else
    { abl0 *= abl1/(abl1+abl2);
      t1 = t2;
      abl1 = abl2;
      dist1 = dist2;
    }
  }
#ifdef MOTION
  fprintf(stderr,"Ergebnis: ");
  fprintf(stderr,"t0=%20.15e dist0=%20.15e abl0=%20.15e ", t0, dist0, abl0);
  fprintf(stderr,"t1=%20.15e dist1=%20.15e abl1=%20.15e\n", t1, dist1, abl1);
#endif
  t2 = (t0+t1)/2;
  mach_tippunkt(t2, tp);
  tp.punkt_welt_tippunkt(point_wrld, point_tool);
  dist2 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
  tp.vektor_tippunkt_welt(grad_tool, grad_wrld);
  geschw = speed(t2, wnp_tool, geschw_v);
  abl2 = -skalarprodukt3(grad_wrld, geschw_v);

  *t_min = t2;
  *dist_min = dist2;

  return 0;
}

int SweepDist::Motion::min_dist
( Werkzeugscheibe &werkzeug,
  double point_wrld[],
  double *t_min,
  double *dist_min,
  double grad_wrld[],

  double t0,
  double t1
)
{
#ifdef MOTION
  fprintf(stderr,"Motion: min_dist(Werkzeugscheibe)\n");
#endif

  Tippunkt tp;
  double t2, dist0, dist1, dist2;
  double abl0, abl1, abl2;
  double grad0_wrld[3], grad1_wrld[3], grad2_wrld[3];

  double point_tool[3];
  double wnp_tool[3];
  double grad_tool[3];
  double geschw;
  double geschw_v[3];

  mach_tippunkt(t0, tp);
  tp.punkt_welt_tippunkt(point_wrld, point_tool);
  dist0 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
  if ( dist0 <= 0)
    return -1;
  tp.vektor_tippunkt_welt(grad_tool, grad0_wrld);
  geschw = speed(t0, wnp_tool, geschw_v);
  abl0 = -skalarprodukt3(grad0_wrld, geschw_v);

  if ( fabs(abl0) < EPS_FKT)
  { *t_min = t0;
    *dist_min = dist0;
    for ( int i = 0; i < 3; ++i)
      grad_wrld[i] = grad0_wrld[i];

    return 0;
  }

  if ( abl0 > 0)
  { *t_min = t0;
	*dist_min = dist0;
	for ( int i = 0; i < 3; ++i)
		grad_wrld[i] = grad0_wrld[i];

	return 1;
  }
  mach_tippunkt(t1, tp);
  tp.punkt_welt_tippunkt(point_wrld, point_tool);
  dist1 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
  if ( dist1 <= 0)
    return -1;
  tp.vektor_tippunkt_welt(grad_tool, grad1_wrld);
  geschw = speed(t1, wnp_tool, geschw_v);
  abl1 = -skalarprodukt3(grad1_wrld, geschw_v);

  if ( fabs(abl1) < EPS_FKT)
  { *t_min = t1;
    *dist_min = dist1;
    for ( int i = 0; i < 3; ++i)
      grad_wrld[i] = grad1_wrld[i];

    return 0;
  }
  if ( abl1 < 0)
  { *t_min = t1;
	*dist_min = dist1;
	for ( int i = 0; i < 3; ++i)
		grad_wrld[i] = grad1_wrld[i];

	return 1;
  }

#ifdef MOTION
  fprintf(stderr,"t0=%20.10e  dist0=%20.10e abl0=%20.10e\n", t0, dist0, abl0);
  fprintf(stderr,"t1=%20.10e  dist1=%20.10e abl1=%20.10e\n", t1, dist1, abl1);
  fprintf(stderr,"Pegasus\n");
  int count = 0;
#endif
  while ( fabs(t1-t0) > EPS_T)
  {
#ifdef MOTION
    ++count;
    fprintf(stderr,"Schritt=%d: ", count);
    fprintf(stderr,"t0=%20.15e dist0=%20.15e abl0=%20.15e ", t0, dist0, abl0);
    fprintf(stderr,"t1=%20.15e dist1=%20.15e abl1=%20.15e\n", t1, dist1, abl1);
#endif
    t2 = t1 - abl1 * ( t1 - t0) /(abl1 - abl0);
    mach_tippunkt(t2, tp);
    tp.punkt_welt_tippunkt(point_wrld, point_tool);
    dist2 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
    if ( dist2 <= 0)
      return -1;
    tp.vektor_tippunkt_welt(grad_tool, grad2_wrld);
    geschw = speed(t2, wnp_tool, geschw_v);
    abl2 = -skalarprodukt3(grad2_wrld, geschw_v);

    if ( fabs(abl2) < EPS_FKT)
    { *t_min = t2;
      *dist_min = dist2;
      for ( int i = 0; i < 3; ++i)
        grad_wrld[i] = grad2_wrld[i];

      return 0;
    }


    if ( abl1 * abl2 < 0)
    {
      t0 = t1;
      abl0 = abl1;
      dist0 = dist1;
      t1 = t2;
      abl1 = abl2;
      dist1 = dist2;
    }
    else
    { abl0 *= abl1/(abl1+abl2);
      t1 = t2;
      abl1 = abl2;
      dist1 = dist2;
    }
  }
#ifdef MOTION
  fprintf(stderr,"Ergebnis: ");
  fprintf(stderr,"t0=%20.15e dist0=%20.15e abl0=%20.15e ", t0, dist0, abl0);
  fprintf(stderr,"t1=%20.15e dist1=%20.15e abl1=%20.15e\n", t1, dist1, abl1);
#endif
  t2 = (t0+t1)/2;
  mach_tippunkt(t2, tp);
  tp.punkt_welt_tippunkt(point_wrld, point_tool);
  dist2 = werkzeug.dist_point( point_tool, grad_tool, wnp_tool);
  tp.vektor_tippunkt_welt(grad_tool, grad_wrld);
  geschw = speed(t2, wnp_tool, geschw_v);
  abl2 = -skalarprodukt3(grad_wrld, geschw_v);

  *t_min = t2;
  *dist_min = dist2;

  return 0;
}
