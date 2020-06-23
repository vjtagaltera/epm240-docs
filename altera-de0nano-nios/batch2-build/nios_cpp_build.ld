/*
 * file: nios_cpp_build.ld
 *
 * This linker script is designed to work
 * with the Nios II Monitor for C/C++ programs.
 *
 * The difference between this script and the default nios_build.ld
 * is that the data section is loaded at the specified data address
 * (instead of always loaded in the code section).
 */

ENTRY(_start)

SECTIONS
{
  /* Read-only sections, merged into text segment: */

  . = nasys_program_mem;

  /*
   * Begin the read-only code section here.
   */

  .text      :
  {
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t*)
    *(.gcc_except_table)


    /* all this junk keeps getting in my way. What is it for?
       what is the right thing to do with it? I shove it here
       for now. bleah. monday!
       dvb 2003.11.03
    */

    *(.jcr)
    *(.text.prefix) /* Force prefix to be first */
    *(.init)
    *(.init.*)
    *(.fini)
    *(.fini.*)
  } =0

  . = ALIGN(4);
  _etext = .;

  PROVIDE (etext = .);

  /*
   * Begin the read-only but not-relocated section of
   * memory here.
   */

  _nasys_rodata = .;

  .ctors   : 
  {
    _ctors_begin = .;
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    _ctors_end = .;
  }
   .dtors         :
  {
    _dtors_begin = .;
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    _dtors_end = .;
  }
  .rodata   :
  {
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r*)
  }

  _nasys_rodata_end = .;

  /*
   * --------------------------------------------------
   * the .data section contains initialized and writeable
   * variables. Data section is loaded at the specified address
   * (data NOT loaded in code section).
   */
  
  _nasys_data_source = .;
  _nasys_data_destination = (nasys_program_mem == nasys_data_mem) ? _nasys_data_source : nasys_data_mem;

  .data _nasys_data_destination :
  {
    _data = .;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d*)
    SORT(CONSTRUCTORS)

    . = ALIGN(4);
    _edata = .;
    _nasys_data_destination_end = .;
    PROVIDE (edata = .);
  }

  _gp = ALIGN(16) + 0x7ff0;
  PROVIDE(gp = _gp);
  .got            : { *(.got.plt) *(.got) }
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata          :
  {
    *(.sdata .sdata.* .gnu.linkonce.s.* .eh_frame)
  }
  _edata = .;
  PROVIDE (edata = .);
  _nasys_data_source_end = _nasys_data_source + SIZEOF(.data) + SIZEOF(.got) + SIZEOF(.sdata);

  /*
   * Lastly, the noninitialized storage area.
   * This will start immediately following
   * the initialized data destination address end.
   * This is either right next to the code,
   * if code address = data address, or out in
   * the data memory, if they're different.
   */

  __bss_start = .;
  _nasys_uninitialized_storage = .;

  .sbss           :
  {
    PROVIDE (__sbss_start = .);
    PROVIDE (___sbss_start = .);
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    PROVIDE (__sbss_end = .);
    PROVIDE (___sbss_end = .);
  }

  .bss       :
  {
    _bss = .;
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.scommon)
    . = ALIGN(4);
  }
  _nasys_uninitialized_storage_end = .;

  /*
   * "_end" is used as the start of the mallocable memoryarea
   */

  _end = .;
  PROVIDE (end = .);

  /*
   * To see if you've exceeded memory, you can
   * check the symbols "_end" for the end of all static
   * data memory, and "_etext" for the end of the code,
   * against your memory map. -- dvb
   */

  /*
   * ------------------------------------------------------------
   * dvb say: "I'll leave all this stuff down here exactly
   * as I found it, for debugging info, without
   * understanding it."
   */

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  .debug_ranges   0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* These must appear regardless of  .  */
}

