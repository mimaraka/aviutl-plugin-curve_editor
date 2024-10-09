function setLockedIcon() {
    $('#button-lock i').addClass('fa-lock').removeClass('fa-lock-open');
    $('#button-lock').prop('title', 'カーブの編集はロックされています');
    isLocked = true;
}

function setUnlockedIcon() {
    $('#button-lock i').addClass('fa-lock-open').removeClass('fa-lock');
    $('#button-lock').prop('title', 'カーブは編集可能です');
    isLocked = false;
}

isLocked = false;

$(document).ready(() => {
    setUnlockedIcon();
    if (config.notifyUpdate) {
        fetch('https://api.github.com/repos/mimaraka/aviutl-plugin-curve_editor/releases/latest').then(response => {
            return response.json();
        }).then(data => {
            if (!config.isLatestVersion(data.tag_name)) {
                $('#button-others').prepend('<div class="notification-badge"></div>');
            }
        });
    }
});

$('#button-copy').on('click', () => {
    window.top.postMessage({
        to: 'native',
        command: 'copy'
    }, '*');
});

$('#button-read').on('click', () => {
    window.top.postMessage({
        to: 'native',
        command: 'read'
    }, '*');
});

$('#button-save').on('click', () => {
    window.top.postMessage({
        to: 'native',
        command: 'save'
    }, '*');
});

$('#button-lock').on('click', () => {
    window.top.postMessage({
        to: 'native',
        command: 'lock'
    }, '*');
    if (isLocked) {
        setUnlockedIcon();
    } else {
        setLockedIcon();
    }
    $('#button-clear').prop('disabled', isLocked);
});

$('#button-clear').on('click', () => {
    window.top.postMessage({
        to: 'native',
        command: 'clear'
    }, '*');
});

$('#button-others').on('click', () => {
    window.top.postMessage({
        to: 'native',
        command: 'others'
    }, '*');
});