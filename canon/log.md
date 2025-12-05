เริ่มเเรกด้วย Method 1 เเค่ A B ใน 2 processor เเบ่งครึ่งกันคูณ


จากนั้น Method 2 เริ่มไปดูพวก Scatter, Gather, Broadcast เเบบ 4 processor matrix A จะหั่นตามจำนวน ส่วน ฺB จะส่งไปเต็มๆ 


Method 3 เริ่มใช้ ScatterV, GatherV เพราะมันระบุจำนวนที่จำนวน เเละตำเเหน่งจาก Matrix A ได้เเต่ มันติดเรื่องของ index ที่มันจะเป็น 1 D ก็เลยไปดูเพื่ม เจอเรื่อง Cartesian ที่มันจะเเบ่ง process เป็น grid ได้ก็ลยทำ 


Method 4 เเต่ก่อนหน้านั้นก็ไปแกะ algo เพิ่มเลย ทำเป็น Grid Processor มาเเล้วก็แจก matrix ที่ shift เเล้วหั่นไปตาม process เเต่ละตัว เเละเวลาทำการ shift ส่งเเต่ละ processor มันจะต้องหาพวก virtual grid ที่เป็นเลย rank ของเเต่ละ processor มา shift เเล้วส่ง shift matrix ไป ตอนเเรกนึงภาพของ shift ไม่ออกเลยไปดูเพิ่ม ลอง proof ดูจะไม่เปลี่ยน size ของ matrix เเต่จะเลือก pivot เเล้วเริ่มจากตำแหน่งนั้นแทน เลยทำออกมาได้  