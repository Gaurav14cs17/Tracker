 public bool isMovingForward;
     public bool isMovingBackward;
     public Vector3 LastPOS;
     public Vector3 NextPOS;
     
 
     void LateUpdate() {
         
         NextPOS.x = transform.position.x;
         
         
         
         if(LastPOS.x < NextPOS.x){
                 isMovingForward = true;
                 isMovingBackward = false;
         }
         if(LastPOS.x > NextPOS.x){
                 isMovingBackward = true;
                 isMovingForward = false;
         }
         else if(LastPOS.x == NextPOS.x){
                 isMovingForward = false;
                 isMovingBackward = false;
         }
         
         LastPOS.x = NextPOS.x;
     }
