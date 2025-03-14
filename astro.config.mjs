// @ts-check
import { defineConfig } from 'astro/config';
import { symlinkIntegration } from './website/src/integrations/symlink';

// https://astro.build/config
export default defineConfig({
    integrations: [symlinkIntegration()],
    site: 'https://www.mathewmariani.com',
    base: '/c99-engine/',
    srcDir: './website/src',
    publicDir: './website/public',
    vite: {
        resolve: {
            preserveSymlinks: true,
        },
    },
});