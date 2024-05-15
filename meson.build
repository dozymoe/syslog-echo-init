project('syslog-echo-init', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

executable('syslog-echo-init',
           'main.cpp',
           # comment this line if you want smaller binary (no static linking)
           # 2.5MB with static linking, 220KB without
           link_args : ['-static'],
           install : true)
