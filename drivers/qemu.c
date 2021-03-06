/*******************************************************************
 * Copyright 2021-2080 evilbinary
 * 作者: evilbinary on 01/01/20
 * 邮箱: rootdebug@163.com
 ********************************************************************/
#include "vga.h"

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH 0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET 0x8
#define VBE_DISPI_INDEX_Y_OFFSET 0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xa

#define VBE_DISPI_ID0 0xB0C0
#define VBE_DISPI_ID1 0xB0C1
#define VBE_DISPI_ID2 0xB0C2
#define VBE_DISPI_ID4 0xB0C4
#define VBE_DISPI_ID5 0xB0C5

#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80

void qemu_write_reg(u16 index, u16 data) {
  io_write16(VBE_DISPI_IOPORT_INDEX, index);
  io_write16(VBE_DISPI_IOPORT_DATA, data);
}

u16 qemu_read_reg(u16 index) {
  io_write16(VBE_DISPI_IOPORT_INDEX, index);
  return io_read16(VBE_DISPI_IOPORT_DATA);
}

void qemu_clear_screen(vga_device_t* vga) {}

void qemu_init_mode(pci_device_t* pdev, vga_device_t* vga, int mode) {
  if (mode == VGA_MODE_80x25) {
    vga->width = 80;
    vga->height = 25;
  } else if (mode == VGA_MODE_320x200x256) {
    vga->width = 320;
    vga->height = 256;
  } else if (mode == VGA_MODE_1024x768x32) {
    vga->width = 1024;
    vga->height = 768;
    vga->bpp = 32;
  }
  vga->mode = mode;
  vga->write = NULL;
  u32 res_io = pci_dev_read32(pdev, PCI_BASE_ADDR2) & 0xFFFFFFF0;
  u16 id = qemu_read_reg(VBE_DISPI_INDEX_ID);
  if ((id & 0xfff0) != VBE_DISPI_ID0) {
    kprintf("qemu get error\n");
  }
  qemu_write_reg(VBE_DISPI_INDEX_ENABLE, 0);
  qemu_write_reg(VBE_DISPI_INDEX_BANK, 0);
  qemu_write_reg(VBE_DISPI_INDEX_BPP, vga->bpp);
  qemu_write_reg(VBE_DISPI_INDEX_XRES, vga->width);
  qemu_write_reg(VBE_DISPI_INDEX_YRES, vga->height);
  qemu_write_reg(VBE_DISPI_INDEX_VIRT_WIDTH, vga->width);
  qemu_write_reg(VBE_DISPI_INDEX_VIRT_HEIGHT, vga->height);
  qemu_write_reg(VBE_DISPI_INDEX_X_OFFSET, 0);
  qemu_write_reg(VBE_DISPI_INDEX_Y_OFFSET, 0);
  qemu_write_reg(VBE_DISPI_INDEX_ENABLE,
                 VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
  qemu_write_reg(0, 0x20);

  qemu_clear_screen(vga);
}

void qemu_init_device(device_t* dev) {
  // pci_device_t* pdev = pci_find_class(0x300);
  pci_device_t* pdev = pci_find_vendor_device(0x1234, 0x1111);
  if (pdev == NULL) {
    kprintf("can not find pci device\n");
    return;
  }
  u32 bar0 = pci_dev_read32(pdev, PCI_BASE_ADDR0) & 0xFFFFFFF0;

  // kprintf("bar0:%x ", bar0);
  vga_device_t* vga = kmalloc(sizeof(vga_device_t));
  vga->frambuffer = bar0;
  dev->data = vga;
  u32 addr = bar0;

  u32 mem = qemu_read_reg(VBE_DISPI_INDEX_VIDEO_MEMORY_64K) * 64 * 1024;
  for (int i = 0; i < mem / 0x1000; i++) {
    map_page(addr, addr, PAGE_P | PAGE_USU | PAGE_RWW);
    addr += 0x1000;
  }
  qemu_init_mode(pdev, vga, VGA_MODE_1024x768x32);
}

int qemu_init(void) {
  device_t* dev = kmalloc(sizeof(device_t));
  dev->name = "qemu";
  dev->read = vga_read;
  dev->write = vga_write;
  dev->ioctl = vga_ioctl;
  dev->id = DEVICE_VGA_QEMU;
  dev->type = DEVICE_TYPE_VGA;
  device_add(dev);

  qemu_init_device(dev);
  return 0;
}

void qemu_exit(void) { kprintf("vga exit\n"); }

module_t qemu_module = {.name = "vga", .init = qemu_init, .exit = qemu_exit};

