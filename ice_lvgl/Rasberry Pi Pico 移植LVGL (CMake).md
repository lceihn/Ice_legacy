## Rasberry Pi Pico 移植LVGL (CMake)

- 获取 **lvgl** 源码  `git clone https://github.com/lvgl/lvgl.git`

- 复制 **lvgl** 文件夹到工程目录下, 将 **lv_conf_template.h** 复制一份并重命名为**lv_conf.h**, 最后将文件里的的 `if 0`改为`if 1`并且修改对应的颜色位数， `#define LV_COLOR_DEPTH 16`

- Include **lvgl/lvgl.h** 在需要使用LVGL相关函数的文件中

```c
  //lvgl init
      lv_init();
      /*A static or global variable to store the buffers*/
      static lv_disp_draw_buf_t disp_buf;
  /*Static or global buffer(s). The second buffer is optional*/
      static lv_color_t buf_1[LCD_W * 50]; //绘制缓冲区大小
      static lv_color_t buf_2[LCD_W * 50];
  /*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
      lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, LCD_W*50);
  
      static lv_disp_drv_t disp_drv;
      lv_disp_drv_init(&disp_drv);
      disp_drv.draw_buf = &disp_buf;
      disp_drv.hor_res = LCD_W; //lcd width
      disp_drv.ver_res = LCD_H; //lcd height
      disp_drv.flush_cb = my_flush_cb; //Set your driver function
      lv_disp_drv_register(&disp_drv);

void my_flush_cb(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    /*It's a very slow but simple implementation.
     *`set_pixel` needs to be written by you to a set pixel on the screen*/
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            lcd_draw_point(x, y, color_p->full);
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}
```

- main函数中while循环里添加

 ```c
      sleep_ms(1);
      lv_task_handler();
      lv_tick_inc(1);
 ```

  