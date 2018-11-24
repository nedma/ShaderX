// Barnsley 2
if(z.x*Seed.y+Seed.x*z.y>=0){
  z=m_c_mul(z-float2(1,0),Seed);
}
else{
  z=m_c_mul(z+float2(1,0),Seed);

}
  