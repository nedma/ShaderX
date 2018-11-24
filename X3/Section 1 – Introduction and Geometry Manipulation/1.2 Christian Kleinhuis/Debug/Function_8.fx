// Barnsley 3
if(z.x > 0){
  z=m_c_power2(z)-float2(1,0);
}
else{
  z=m_c_power2(z)-float2(1,0)+Seed*z.x;

} 
