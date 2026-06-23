import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
    base: './',                 // file:// 読み込みのため相対パス出力
    plugins: [react()],
    build: {
        outDir: 'dist',
        emptyOutDir: true,
    },
});
