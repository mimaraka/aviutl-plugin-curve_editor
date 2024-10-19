'use strict';

let isLocked = false;

window.addEventListener('message', function(event) {
    if (event.data.to == 'toolbar') {
        switch (event.data.command) {
            case 'changeEditMode':
                updateReadButton();
                break;
        }
    }
});

const setLockedIcon = () => {
    $('#button-lock i').addClass('fa-lock').removeClass('fa-lock-open');
    $('#button-lock').prop('title', 'カーブの編集はロックされています');
    isLocked = true;
}

const setUnlockedIcon = () => {
    $('#button-lock i').addClass('fa-lock-open').removeClass('fa-lock');
    $('#button-lock').prop('title', 'カーブは編集可能です');
    isLocked = false;
}

const updateReadButton = () => {
    $('#button-read').prop('disabled', !(config.editMode == 2 || config.editMode == 3 || config.editMode == 4));
}

$(document).ready(() => {
    setUnlockedIcon();
    updateReadButton();
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