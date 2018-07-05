/* Copyright (C) 2012-2017 IBM Corp.
 * This program is Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */
#if 0
#include <cstdio>
#include <cassert>
#if (defined(__unix__) || defined(__unix) || defined(unix))
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <NTL/BasicThreadPool.h>
#endif
#include <NTL/ZZ.h>
NTL_CLIENT
#include "norms.h"
#include "EncryptedArray.h"

#ifdef DEBUG_PRINTOUT
#include "debugging.h"
#endif

int main(int argc, char *argv[]) 
{
  ArgMapping amap;

  long m=16;
  amap.arg("m", m, "cyclotomic index");
  amap.note("e.g., m=1024, m=2047");

  long r=8;
  amap.arg("r", r, "bit of precision");
  amap.parse(argc, argv);

  vector<long> f;
  factorize(f,m);
  cout << "r="<<r<<", factoring "<<m<<" gives [";
  for (unsigned long i=0; i<f.size(); i++)
    cout << f[i] << " ";
  cout << "]\n";

  FHEcontext context(m, /*p=*/-1, r);
  buildModChain(context, 5, 2);

  const EncryptedArray& ea = *context.ea;
  ea.getPAlgebra().printout();

#ifdef DEBUG_PRINTOUT
  vector<cx_double> vc1;
  ea.random(vc1);
  cout << "random complex vc1=";
  printVec(cout,vc1,8)<<endl;

  vector<double> vd;
  ea.random(vd);
  cout << "random real vd=";
  printVec(cout,vd,8)<<endl;
#endif

  vector<long> vl;
  ea.random(vl);
  vl[1] = -1; // ensure that this is not the zero vector
#ifdef DEBUG_PRINTOUT
  cout << "random int v=";
  printVec(cout,vl,8)<<endl;
#endif

  zzX poly;
  ea.encode(poly, vl);
  ZZX poly2;
  convert(poly2, poly);
  cout << "  encoded into a degree-"<<NTL::deg(poly2)<<" polynomial\n";

  vector<double> vd2;
  ea.decode(vd2, poly2);
#ifdef DEBUG_PRINTOUT
  cout << "  decoded into vd2=";
  printVec(cout,vd2,8)<<endl;
#endif
  assert(lsize(vl)==lsize(vd2));

  double maxDiff = 0.0;
  for (long i=0; i<lsize(vl); i++) {
    double diffAbs = std::abs(vl[i]-vd2[i]);
    if (diffAbs > maxDiff)
      maxDiff = diffAbs;
  }
  cout << "  max |v-vd2|_{infty}="<<maxDiff<<endl;
  return 0;
}
