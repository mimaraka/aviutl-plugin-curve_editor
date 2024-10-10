const handleMessage = event => {
    switch (event.data.to) {
        case 'native':
            window.chrome.webview.postMessage(event.data);
            break;

        case 'toolbar':
            $('#toolbar')[0].contentWindow.postMessage(event.data, '*');
            break;

        case 'panel-editor':
        case 'editor-graph':
            $('#panel-editor')[0].contentWindow.postMessage(event.data, '*');
            break;
    }
}

window.addEventListener('message', handleMessage);
window.chrome.webview.addEventListener('message', handleMessage);

const disablePanel = () => {
    $('body').append('<div id="disabled-overlay"></div>');
    $('#disabled-overlay').on('mousedown', () => {
        window.chrome.webview.postMessage({
            to: 'native',
            command: 'flash'
        });
    });
}

const enablePanel = () => {
    $('#disabled-overlay').remove();
}

const onDrag = () => {
    $('#button-apply').html('トラックバーにドラッグ&ドロップして適用');
    window.chrome.webview.postMessage({
        to: 'native',
        command: 'drag-and-drop'
    });
}

const onDrop = () => {
    $('#button-apply').html('適用');
}

$('#button-apply').on('mousedown', () => {
    onDrag();
});

$(window).on('mouseup', () => {
    onDrop();
});

let isDragging = false;
let separatorPos = 1;
const menuHeight = Number($('#toolbar').css('height').replace('px', ''));
const separatorHeight = Number($('#separator').css('height').replace('px', ''));
const applyButtonHeight = Number($('#button-apply').css('height').replace('px', '')) + Number($('#button-apply').css('margin-top').replace('px', '')) + Number($('#button-apply').css('margin-bottom').replace('px', ''));

$('#separator').on('mousedown', () => {
    isDragging = true;
    $('iframe').css('pointer-events', 'none');

});

$(window).on('mousemove', event => {
    if (isDragging) {
        const tmp = document.documentElement.clientHeight - menuHeight * 2 - applyButtonHeight - separatorHeight;
        const tmp2 = (event.clientY - menuHeight * 2 - separatorHeight * 0.5 - applyButtonHeight) / tmp;
        separatorPos = Math.max(Math.min(tmp2, 1), 0);

        $('#panel-editor').css('height', `${menuHeight + separatorHeight * 0.5 + tmp * separatorPos - separatorHeight * 0.5}px`);
    }
});

$(window).on('mouseup', () => {
    isDragging = false;
    $('iframe').css('pointer-events', 'auto');
});

$(window).on('resize', () => {
    const tmp = document.documentElement.clientHeight - menuHeight * 2 - applyButtonHeight - separatorHeight;
    $('#panel-editor').css('height', `${menuHeight + separatorHeight * 0.5 + tmp * separatorPos}px`);
});
