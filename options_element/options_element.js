// $Id$

/**
 * @file
 * Add JavaScript behaviors for the "options" form element type.
 */
Drupal.behaviors.optionsElement = function(context) {
  $('fieldset.form-options:not(.options-element-processed)', context).each(function() {
    $(this).addClass('options-element-processed');
    new Drupal.optionsElement(this);
  });
};

/**
 * Constructor for an options element.
 */
Drupal.optionsElement = function(element) {
  var self = this;

  // Find the original "manual" fields.
  this.manualElement = element;
  this.manualOptionsElement = $(element).find('textarea').get(0);
  this.manualDefaultValueElement = $(element).find('input').get(0);

  // Setup variables containing the current status of the widget.
  this.optgroups = $(element).is('.options-optgroups');
  this.multiple = $(element).is('.options-multiple');
  this.keyType = element.className.replace(/^.*?options-key-type-([a-z]+).*?$/, '$1');
  this.identifier = this.manualOptionsElement.id + '-widget';

  // Setup new DOM elements containing the actual options widget.
  this.optionsElement = $('<div></div>').get(0); // Temporary DOM object. 
  this.optionsToggleElement = $(Drupal.theme('optionsElementToggle')).get(0);

  // Add the options widget and toggle elements to the page.
  $(element).hide().before(this.optionsElement).after(this.optionsToggleElement);

  // Add a toggle action for manual entry of options.
  $(this.optionsToggleElement).find('a').click(function(){
    self.toggleMode();
    return false;
  });

  // Update the options widget with the current state of the textarea.
  this.updateWidgetElements();
}

/**
 * Update the widget element based on the current values of the manual elements.
 */
Drupal.optionsElement.prototype.updateWidgetElements = function () {
  var self = this;

  // Create a new options element and replace the existing one.
  var newElement = $(Drupal.theme('optionsElement', this)).get(0);
  $(this.optionsElement).replaceWith(newElement);
  this.optionsElement = newElement;

  // Manually setup table drag for the created table.
  Drupal.settings.tableDrag = Drupal.settings.tableDrag || {};
  Drupal.settings.tableDrag[this.identifier] = {
    'option-depth': {
      0: {
        action: 'depth',
        hidden: false,
        limit: 0,
        relationship: 'self',
        source: 'option-depth',
        target: 'option-depth',
      }
    }
  };

  // Allow indentation of elements if optgroups are supported.
  if (this.optgroups) {
    Drupal.settings.tableDrag[this.identifier]['option-parent'] = {
      0: {
        action: 'match',
        hidden: false,
        limit: 1,
        relationship: 'parent',
        source: 'option-key',
        target: 'option-parent',
      }
    };
  }

  // Enable button for adding options.
  $('a.add', this.optionElement).click(function() {
    self.addOption($(this).parents('tr:first').get(0));
    return false;
  });

  // Enable button for removing options.
  $('a.remove', this.optionElement).click(function() {
    self.removeOption($(this).parents('tr:first').get(0));
    return false;
  });

  // Add the same update action to all textfields and radios.
  $('input', this.optionsElement).change(function() {
    self.updateOptionElements();
    self.updateManualElements();
  });

  // Add a delayed update to textfields.
  $('input.option-value', this.optionsElement).keyup(function(e) {
    self.pendingUpdate(e);
  });

  // Attach behaviors as normal to the new widget.
  Drupal.attachBehaviors(this.optionsElement);

  // Add an onDrop action to the table drag instance.
  Drupal.tableDrag[this.identifier].onDrop = function() {
    // Update the checkbox/radio buttons for selecting default values.
    if (self.optgroups) {
      self.updateOptionElements();
    }
    // Update the options within the hidden text area.
    self.updateManualElements();
  };

  // Set the tab indexes.
  this.updateOptionElements();
}

/**
 * Update the original form elment based on the current state of the widget.
 */
Drupal.optionsElement.prototype.updateManualElements = function() {
  var options = {};

  // Build a list of current options.
  var previousOption = false;
  $(this.optionsElement).find('input.option-value').each(function() {
    var $row = $(this).is('tr') ? $(this) : $(this).parents('tr:first');
    var depth = $row.find('input.option-depth').val();
    if (depth == 1 && previousOption) {
      if (typeof(options[previousOption]) != 'object') {
        options[previousOption] = {};
      }
      options[previousOption][this.value] = this.value;
    }
    else {
      options[this.value] = this.value;
      previousOption = this.value;
    }
  });
  this.options = options;

  // Update the default value.
  var defaultValue = this.multiple ? [] : '';
  var multiple = this.multiple;
  $(this.optionsElement).find('input.option-default').each(function() {
    if (this.checked) {
      if (multiple) {
        defaultValue.push(this.value);
      }
      else {
        defaultValue = this.value;
      }
    }
  });
  this.defaultValue = defaultValue;

  // Update with the new text and trigger the change action on the field.
  this.optionsToText();
  $(this.manualOptionsElement).change();
}

/**
 * Several maintenance routines to update all rows of the options element.
 *
 * - Disable options for optgroups if indented.
 * - Disable add and delete links if indented.
 * - Match the default value radio button value to the key of the text element.
 * - Reset the taborder.
 */
Drupal.optionsElement.prototype.updateOptionElements = function() {
  var self = this;
  var previousElement = false;

  $(this.optionsElement).find('tr').each(function(index) {
    // Update the elements key if matching the key and value.
    if (self.keyType == 'associative') {
      var optionValue = $(this).find('input.option-value').val();
      $(this).find('input.option-key').val(optionValue);
    }
    // Match the default value checkbox/radio button to the option's key.
    var optionKey =$(this).find('input.option-key').val();
    $(this).find('input.option-default').val(optionKey);

    // Set the tab order.
    $(this).find('input.form-text').attr('tabindex', index + 1);

    // Adjust the row if indented.
    var depth = $(this).find('input.option-depth').val();
    var defaultInput = $(this).find('input.option-default').get(0);

    if (depth == 1) {
      $(previousElement).attr('disabled', true).attr('checked', false);
      $(previousElement).parent().find('a.add, a.remove').hide();
      $(defaultInput).attr('disabled', false);
    }
    else {
      $(defaultInput).attr('disabled', false);
      $(defaultInput).parent().find('a.add, a.remove').show();
      previousElement = defaultInput;
    }
  });
}

/**
 * Add a new option below the current row.
 */
Drupal.optionsElement.prototype.addOption = function(currentOption) {
  var self = this;
  var newOption = $(currentOption).clone()
    .find('input.option-value').val('').end()
    .find('input.option-default').attr('checked', false).end()
    .find('a.tabledrag-handle').remove().end()
    .removeClass('drag-previous')
    .insertAfter(currentOption)
    .get(0);

  // Make the new option draggable.
  Drupal.tableDrag[this.identifier].makeDraggable(newOption);

  // Enable button for adding options.
  $('a.add', newOption).click(function() {
    self.addOption(newOption);
    return false;
  });

  // Enable buttons for removing options.
  $('a.remove', newOption).click(function() {
    self.removeOption(newOption);
    return false;
  });

  // Add the update action to all textfields and radios.
  $('input', newOption).change(function() {
    self.updateOptionElements();
    self.updateManualElements();
  });

  // Add a delayed update to textfields.
  $('input.option-value', newOption).keyup(function(e) {
    self.pendingUpdate(e);
  });

  this.updateOptionElements();
  this.updateManualElements();
}

/**
 * Remove the current row.
 */
Drupal.optionsElement.prototype.removeOption = function(currentOption) {
  $(currentOption).remove();

  this.updateOptionElements();
  this.updateManualElements();
}

/**
 * Toggle link for switching between the JavaScript and manual entry.
 */
Drupal.optionsElement.prototype.toggleMode = function() {
  if ($(this.optionsElement).is(':visible')) {
    $(this.optionsElement).hide();
    $(this.manualElement).show();
    $(this.optionsToggleElement).find('a').html(Drupal.t('Normal entry'));
  }
  else {
    this.updateWidgetElements();
    $(this.optionsElement).show();
    $(this.manualElement).hide();
    $(this.optionsToggleElement).find('a').html(Drupal.t('Manual entry'));
  }
}

/**
 * Update a field after a delay.
 *
 * Similar to immediately changing a field, this field as pending changes that
 * will be updated after a delay. This includes textareas and textfields in
 * which updating continuously would be a strain the server and actually slow
 * down responsiveness.
 */
Drupal.optionsElement.prototype.pendingUpdate = function(e) {
  var self = this;

  // Only operate on "normal" keys, excluding special function keys.
  // http://protocolsofmatrix.blogspot.com/2007/09/javascript-keycode-reference-table-for.html
  if (!(
    e.keyCode >= 48 && e.keyCode <= 90 || // 0-9, A-Z.
    e.keyCode >= 93 && e.keyCode <= 111 || // Number pad.
    e.keyCode >= 186 && e.keyCode <= 222 || // Symbols.
    e.keyCode == 8) // Backspace.
    ) {
    return;
  }

  if (this.updateDelay) {
    clearTimeout(this.updateDelay);
  }

  this.updateDelay = setTimeout(function(){
    self.updateOptionElements();
    self.updateManualElements();
  }, 500);
};

/**
 * Given an object of options, convert it to a text string.
 */
Drupal.optionsElement.prototype.optionsToText = function() {
  var $rows = $('tr', this.optionsElement);
  var output = '';
  var previousParent = 0;
  var rowCount = $rows.size();
  var defaultValues = [];

  // Loop through rows in reverse to find parents easier.
  for (var rowIndex = rowCount - 1; rowIndex >= 0; rowIndex--) {
    var indent = $rows.eq(rowIndex).find('input.option-depth').val();
    var key = $rows.eq(rowIndex).find('input.option-key').val();
    var value = $rows.eq(rowIndex).find('input.option-value').val();
    var parent = $rows.eq(rowIndex).find('input.option-parent').val();
    var checked = $rows.eq(rowIndex).find('input.option-default').attr('checked');

    // Add to default values.
    if (checked) {
      defaultValues.push(key);
    }

    // Handle groups.
    if (key == previousParent) {
      output = '<' + key + '>' + "\n" + output;
      previousParent = 0;
    }
    // Typical key|value pairs.
    else {
      // Exit out of any groups.
      if (previousParent != parent && parent != 0) {
        output = "<>\n" + output;
      }
      if (this.keyType == 'none') {
        output = value + "\n" + output;
      }
      else if (key == '' && value == '') {
        output = "\n" + output;
      }
      else {
        output = key + '|' + value + "\n" + output;
      }
      previousParent = parent;

    }
  }

  this.manualOptionsElement.value = output;
  this.manualDefaultValueElement.value = defaultValues.join(', ');
};

/**
 * Given a text string, convert it to an object.
 */
Drupal.optionsElement.prototype.optionsFromText = function() {
  var rows = this.manualOptionsElement.value.match(/^.*$/mg);
  var parentKey = 0;
  var options = [];
  var defaultValues = {};

  // Drop the last row if empty.
  if (rows.length && rows[rows.length - 1] == '') {
    rows.pop();
  }

  if (this.multiple) {
    var defaults = this.manualDefaultValueElement.value.split(',');
    for (var n in defaults) {
      var defaultValue = defaults[n].replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
      defaultValues[defaultValue] = defaultValue;
    }
  }
  else {
    var defaultValue = this.manualDefaultValueElement.value.replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
    defaultValues[defaultValue] = defaultValue;
  }

  for (var n in rows) {
    var row = rows[n].replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
    var key = '';
    var value = '';
    var checked = false;
    var hasChildren = false;
    var groupClear = false;

    var matches = {};
    // Row is a group.
    if (this.optgroups && (matches = row.match(/^\<([^>]*)\>$/))) {
      if (matches[0] == '<>') {
        parentKey = 0;
        groupClear = true;
      }
      else {
        parentKey = key = value = matches[1];
        hasChildren = true;
      }
    }
    // Row is a key|value pair.
    else if (matches = row.match(/^([^|]+)\|(.*)$/)) {
      key = matches[1];
      value = matches[2];
    }
    // Row is a straight value.
    else {
      key = row;
      value = row;
    }

    if (!groupClear) {
      options.push({
        key: key,
        value: value,
        parent: (key !== parentKey ? parentKey : 0),
        hasChildren: hasChildren,
        checked: (defaultValues[key] ? 'checked' : false)
      });
    }
  }

  return options;
}

/**
 * Theme function for creating a new options element.
 *
 * @param optionsElement
 *   An options element object.
 */
Drupal.theme.prototype.optionsElement = function(optionsElement) {
  var output = '';
  var options = optionsElement.optionsFromText();
  var defaultType = optionsElement.multiple ? 'checkbox' : 'radio';

  // Helper function to print out a single draggable option row.
  function tableDragRow(key, value, parentKey, indent, status) {
    var output = '';
    output += '<tr class="draggable">'
    output += '<td>';
    for (var n = 0; n < indent; n++) {
      output += Drupal.theme('tableDragIndentation');
    }
    output += '<input type="hidden" class="option-key" value="' + key + '" />';
    output += '<input type="hidden" class="option-parent" value="' + parentKey + '" />';
    output += '<input type="hidden" class="option-depth" value="' + indent + '" />';
    output += '<input type="' + defaultType + '" name="' + optionsElement.identifier + '-default" class="form-radio option-default" value="' + key + '"' + (status == 'checked' ? ' checked="checked"' : '') + (status == 'disabled' ? ' disabled="disabled"' : '') + ' />';
    output += '<input class="form-text option-value" type="text" value="' + value + '" />';
    output += '<a class="add" title="' + Drupal.t('Add new option') + '" href="#"' + (status == 'disabled' ? ' style="display: none"' : '') + '><span class="add">' + Drupal.t('Add') + '</span></a>';
    output += '<a class="remove" title="' + Drupal.t('Remove option') + '" href="#"' + (status == 'disabled' ? ' style="display: none"' : '') + '><span class="remove">' + Drupal.t('Remove') + '</span></a>';
    output += '</td>';
    output += '</tr>';
    return output;
  }

  output += '<div class="options-widget">';
  output += '<table id="' + optionsElement.identifier + '">';
  output += '<tbody>';

  for (var n in options) {
    var option = options[n];
    var depth = option.parent == 0 ? 0 : 1;
    if (option.hasChildren) {
      output += tableDragRow(option.key, option.key, option.parent, depth, 'disabled');
    }
    else {
      output += tableDragRow(option.key, option.value, option.parent, depth, option.checked);
    }
  }

  output += '</tbody>';
  output += '</table>';
  output += '<div>';

  return output;
}

Drupal.theme.prototype.optionsElementToggle = function() {
  return '<div class="form-options-manual"><a href="#">' + Drupal.t('Manual entry') + '</a></div>';
}

Drupal.theme.tableDragChangedMarker = function () {
  return ' ';
};

Drupal.theme.tableDragChangedWarning = function() {
  return ' ';
}
