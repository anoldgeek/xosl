# xosl
Extended Operating System Loader 1.3.0 alpha 22. 
An update/branch of the orignal described at https://groups.yahoo.com/neo/groups/xosl/info

PLEASE READ THE FOLLOWING NOTES CAREFULLY, BEFORE INSTALLING XOSL

Extended Operating System Loader comes with ABSOLUTELY NO WARRANTY, it
is distributed under the terms of the GNU General Public License (GPL).
The full license can be found in the GPL.TXT file.

1. This version additionally supports:
   a) New start-up options on install.
      -o <drive-offset>
         Enables installing from a USB device that has booted as C:
         Use '-o 1' to direct xosl install to ignore the first drive.
      -p <file-path>
         Choose the folder to save/restore created files when installing 
         or upgrading. e.g.
         Use -p C:\temp\ to direct all created files to c:\temp
         Use -p E:\backups\<this-pc-name>\<this-pc-partition> to save 
         xosl backups from multiple systems to the same base folder e.g.
                install -p e:\xoslbup\freds-pc\sda1
         Use -p C:\temp\ when installing to a specifically created dedicated
         partition from a 1.44MB floppy. So that no additional space is 
         required on the floppy. The folder must already exist.
      -b <enable-disable-part-backup-data>  when installing to a 
         dedicated partition. e.g.
         '-b 1' Backup the partition details and data.
         '-b 0' Only backup the partition details.
         Use '-b 0' when installing to a specifically created dedicated 
         partition from a 1.44MB floppy. So that the save requires an 
         additional 512 bytes rather than approx 478,000.

   b) > 2TB hard disks are supported provided LBA is available and is 
      GPT Partitioned.
   c) GPT partitioned disks have been tested with grub2 and xosl chain load.
   d) Upgrading  from older versions >= 1.1.4
   e) When installing on a dedicated partition, the option the choose 
      which HD, or none, the sector 0 MBR boot code is written to.
      Choosing option NONE allows additional installations of XOSL for 
      chain loading without disturbing the existing install.
   f) When installing on a FAT partition, the option to choose
      the HD the sector 0 MBR boot code is written to.
      Choosing option NONE allows additional installations of XOSL for 
      chain loading without disturbing the existing install.
   
2. 'Easy upgrade' is now supported for installs later than XOSL 1.1.4. 
older versions will have to be completely reinstalled from scratch.
However please do not rely on this. Should this fail be prepared to 
reinstall from scratch.

3. If you have any version older than 1.1.4 installed, it is recommended
to uninstall it using the existing version of XOSL before installing the 
latest. To perform a quick uninstall via DOS, execute 'fdisk /mbr'.

4. It is strongly recommended to install XOSL from a usb stick or CD. 
Using a usb stick for dedicated install you can save a backup of the 
data overwritten.

5. With XOSL comes Ranish Partition Manager, written by Mikhail
Ranish. 
This should not be used with modern (Advanced format) or GPT 
partitioned HDs.
Note that certain circumstances exist where it is not
free to use. Refer to http://www.ranish.com/part for the details.

6. With XOSL comes Smart Boot Manager (SBM). It is distributed 
under the terms of the GNU GPL. XOSL use SBM to indirectly 
support booting from CD-ROM. The CD-ROM booting howto can be
found in the manual/cdrom folder. The manual to SBM can be 
found in the manual/sbm folder. SBM can be found at 
http://btmgr.sourceforge.net. When an update of SBM is available,
and you want to install it, you will have to replace SBMINST.EXE,
as found in this distribution, with the new one, and completely 
reinstall XOSL.

-- NOTES ON INSTALLATION ON DOS DRIVE --

1. If installing from a usb device that mounts as C:, use the
   -o <drive-offset> option. i.e. 'install -o 1' to hide the usb
   device.

-- NOTES ON INSTALLATION ON DEDICATED PARTITION --

1. You have to create a partition for XOSL yourself, BEFORE you
run install. The XOSL install utility does not create a partition
for you. If you are sure that you will not need to recover the
partition data, you can use the '-b 0' option to save only the 
partition details. This requires only 512 bytes instead of 478kb.


2. Select the partition you want to install XOSL onto carefully.
ALL DATA ON THE SELECTED PARTITION WILL BE DESTROYED! Although 
install provides an uninstall option, which can completely restore
the partition data, you should NEVER rely on such features. 

3. If you install from floppy or usb (recommended), make sure that the
device has at least 879kb (900102 byte) of free disk space BEFORE you run
install (and after you have copied the XOSL install files to the 
device) or use the -p option. If you don't install Ranish Partition 
Manager, only 816kb (834566 byte) of free disk space is required. 
If you use the opt not to backup the partition data, '-b 0', this is 
reduced by approx 478kb.
Generally backup of partition data is not possible on 1.44Mb floppies as 
they are not large enough. Tested OK with 2.8Mb. You can also use the -p
option to direct the backup to another drive. 
e.g. 'install -p C:\PARTBACK\'

4.  If installing from a usb device that mounts as C:, use the
   -o <drive-offset> option. i.e. 'install -o 1' to hide the usb
   device.

-- Fixes/enhancements since xosl 115 --

   a) Fixes since XOSL115 in 116 and 118 have been applied.
   b) StartSector is now 'unsigned long long' to cope with HDs > 2TB.
   c) The partition display size uses suffix of M, G & T.
   d) Date/time settings of XOSL files are set/updated.
   e) An upgrade option has added.
   f) Command line options for install.exe have been added.
   g) The ability to store the MBR to HD's other than HD0 has been added.
   h) The partlist is updated when returning to main menu after installing
      XOSL.
   i) The code has been migrated to Openwatcom 2.0. 
      Because 'unsigned long long' (64bit long) is required for >2T disks.
   j) Add boot from GPT disk via bios (msdos). However, this has only been 
      tested with grub2 and XOSL FS chain load.
   k) Increased the  size of BootItemsFile to allow for more partitions. 
      A GPT disk has a default and max of 128 partitions.
   l) Fixed xosl boot to work with disks > 2TB
 
-- Known limitations -- 
   a) There are likely to be many more unknown limitations as testing has
      been limited to the authors hardware and software.
   b) UEFI as not been implemented. 
      So Windows and Linux cannot be booted directly on GPT disks but can be
      chain loaded via grub2.
   c) Neither ext2, ext3 or ext4 filesystems are implemented so Linux must 
      be booted via grub2 or grub legacy.
