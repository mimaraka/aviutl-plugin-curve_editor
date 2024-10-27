const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');
const TerserPlugin = require('terser-webpack-plugin');
const path = require('path');

module.exports = {
    entry: './src/index.tsx',
    module: {
        rules: [
            {
                test: /\.(ts|tsx)$/,
                use: 'ts-loader',
                exclude: /node_modules/
            },
            {
                test: /\.(css|scss|sass)$/,
                use: ['style-loader', 'css-loader', 'sass-loader']
            },
            {
                test: /\.ttf$/,
                type: 'asset/resource'
            }
        ]
    },
    plugins: [
        new MonacoWebpackPlugin({
            languages: ['lua']
        })
    ],
    resolve: {
        extensions: ['.tsx', '.ts', '.js']
    },
    optimization: {
        minimizer: [
            new TerserPlugin({
                extractComments: false
            })
        ]
    },
    output: {
        filename: 'bundle.js',
        path: path.resolve(__dirname, 'dist/js')
    }
};