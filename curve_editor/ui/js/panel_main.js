window.addEventListener('message', function(event) {
    if (event.data.to == 'editor-graph') {
        $('#panel-editor')[0].contentWindow.postMessage(event.data, '*');
    }
    this.chrome.webview.postMessage(event.data);
});

const disablePanel = () => {
    $('body').append('<div id="disabled-overlay"></div>');
    $('#disabled-overlay').on('mousedown', function() {
        window.chrome.webview.postMessage({
            command: 'flash'
        });
    });
}

const enablePanel = () => {
    $('#disabled-overlay').remove();
}

$(window).on('mouseup', function() {
    $('#button-apply').html('適用');
});

$('#button-apply').on('mousedown', function() {
    $('#button-apply').html('トラックバーにドラッグ&ドロップして適用');
});

let isDragging = false;
let separatorPos = 1;
const menuHeight = Number($('#toolbar').css('height').replace('px', ''));
const separatorHeight = Number($('#separator').css('height').replace('px', ''));
const applyButtonHeight = Number($('#button-apply').css('height').replace('px', '')) + Number($('#button-apply').css('margin-top').replace('px', '')) + Number($('#button-apply').css('margin-bottom').replace('px', ''));

$('#separator').on('mousedown', function() {
    isDragging = true;
    $('iframe').css('pointer-events', 'none');

});

$(window).on('mousemove', function(event) {
    if (isDragging) {
        const tmp = document.documentElement.clientHeight - menuHeight * 2 - applyButtonHeight - separatorHeight;
        const tmp2 = (event.clientY - menuHeight * 2 - separatorHeight * 0.5 - applyButtonHeight) / tmp;
        separatorPos = Math.max(Math.min(tmp2, 1), 0);

        $('#panel-editor').css('height', `${menuHeight + separatorHeight * 0.5 + tmp * separatorPos - separatorHeight * 0.5}px`);
    }
});

$(window).on('mouseup', function() {
    isDragging = false;
    $('iframe').css('pointer-events', 'auto');
});

$(window).on('resize', function() {
    const tmp = document.documentElement.clientHeight - menuHeight * 2 - applyButtonHeight - separatorHeight;
    $('#panel-editor').css('height', `${menuHeight + separatorHeight * 0.5 + tmp * separatorPos}px`);
});
