const path = require('path');
const fs = require('fs');
const UglifyJS = require('uglify-js');
const CleanCSS = require('clean-css');
const htmlMinifier = require('html-minifier');

const targetDir = process.argv[2];
const minifyInfo = [
    {
        dir: targetDir,
        ext: 'html',
        minify: (content) => htmlMinifier.minify(content, { collapseWhitespace: true, removeComments: true })
    },
    {
        dir: path.join(targetDir, 'css'),
        ext: 'css',
        minify: (content) => new CleanCSS({inline: false}).minify(content).styles
    },
    {
        dir: path.join(targetDir, 'js'),
        ext: 'js',
        minify: (content) => UglifyJS.minify(content).code
    }
]

minifyInfo.forEach(({ dir, ext, minify }) => {
    fs.readdir(dir, (err, files) => {
        if (err) {
            console.error(`Error reading directory: ${err}`);
        } else {
            files.forEach(file => {
                const targetPath = path.join(dir, file);
                const fileExt = path.extname(targetPath).slice(1);
				if (fileExt == ext) {
					console.log(`Minifying ${targetPath}`);
                    fs.writeFileSync(
                        targetPath,
                        minify(fs.readFileSync(targetPath, 'utf8'))
                    );
                }
            });
        }
    });
});