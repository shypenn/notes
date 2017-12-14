" Roney
" add this line in case eslint throws error usually caused by malformated
" eslintrc, or fail silently without any messages.
" 'output_stream': 'both',
call ale#linter#Define('javascript', {
\   'name': 'eslint',
\   'executable_callback': 'ale#handlers#eslint#GetExecutable',
\   'command_callback': 'ale#handlers#eslint#GetCommand',
\   'callback': 'ale#handlers#eslint#Handle',
\   'output_stream': 'both',
\})
