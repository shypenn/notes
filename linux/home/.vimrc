"let &verbose=1
set lines=55 columns=150

" <<don't work in .gvimrc
set go-=m
" >>

set noeb vb t_vb=
set nobk
set tabstop=2
set expandtab
set shiftwidth=2
set autoindent
set cindent
set cinoptions=j1,J1
set incsearch
set hlsearch
set nu
set showcmd
syntax enable
filetype plugin indent on

" turn off Ex mode
nnoremap Q <Nop>

" solarized color
set t_Co=16
set background=dark
let g:solarized_italic=0
colorscheme solarized

" nerdtree
" open if no file specified;
autocmd StdinReadPre * let s:std_in=1
autocmd VimEnter * if argc() == 0 && !exists("s:std_in") | NERDTree | endif
noremap <F2> :NERDTreeToggle<CR>


packadd! matchit

" set json filetype
au BufNewFile, BufRead *.json,*.json5 set filetype=json

" enter current folder when open
au BufEnter * silent! lcd %:p:h

" easy align
" Start interactive EasyAlign in visual mode (e.g. vipga)
xmap ga <Plug>(EasyAlign)
" Start interactive EasyAlign for a motion/text object (e.g. gaip)
nmap ga <Plug>(EasyAlign)

" table mode, markdown style
let g:table_mode_corner='|'


" Tagbar
nmap <F5> :TagbarToggle<CR>


" async lint enging
let g:ale_linters = {
\   'javascript': ['eslint'],
\   'css': ['stylelint']}
let g:ale_fixers = {
\   'javascript': ['prettier-eslint'],
\   'css': ['stylelint'],
\}
let g:ale_javascript_eslint_options="--ignore-pattern '!.*'"
nmap <silent> <C-k> <Plug>(ale_previous_wrap)
nmap <silent> <C-j> <Plug>(ale_next_wrap)
noremap <F3> :ALEFix<CR>

" autoformat
" names defined in auto-format/plugin/defaults.vim
let g:formatters_javascript=['jsbeautify_javascript', 'eslint_local']
noremap <F4> :Autoformat<CR>

" closetag
let g:closetag_filenames = '*.htm,*.html,*.xhtml,*.phtml,*.js,*.jsx'
let g:closetag_xhtml_filenames = '*.htm,*.html,*.xhtml,*.phtml,*.js,*.jsx'

" vim-instant-markdown
let g:instant_markdown_autostart = 0
let g:javascript_plugin_flow = 1

" flow
" 
" call this manually in case being stopped abruptly
" au VimLeave javascript !flow stop
"
" enable check on write: FlowToggle
let g:flow#enable = 0
let g:flow#autoclose = 1
let g:flow#errjmp = 1


" jsx
let g:jsx_ext_required = 0

---WIN_START---
" editorconfig
let g:EditorConfig_exec_path = 'C:\ProgramData\chocolatey\lib\editorconfig.core\tools\editorconfig.exe'
---WIN_END---

execute pathogen#infect()
" call :Helptags to init all docs;
