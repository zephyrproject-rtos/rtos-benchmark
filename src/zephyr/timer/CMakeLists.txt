if(CONFIG_CORTEX_M_SYSTICK)
    target_sources(app PRIVATE bench_cortex_m_systick.c)
elseif(CONFIG_HPET_TIMER)
    target_sources(app PRIVATE bench_hpet.c)
elseif(CONFIG_RISCV_MACHINE_TIMER)
    target_sources(app PRIVATE bench_riscv_machine_timer.c)
else()
    message(FATAL_ERROR "Unknown timer driver selected!")
endif()
