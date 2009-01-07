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
  this.options = Drupal.optionsElement.optionsFromText(this.manualOptionsElement.value);
  this.defaultValue = Drupal.optionsElement.defaultsFromText(this.manualDefaultValueElement.value, this.multiple);
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
  $('input.form-options-value', this.optionsElement).keyup(function(e) {
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
}

/**
 * Update the original form elment based on the current state of the widget.
 */
Drupal.optionsElement.prototype.updateManualElements = function() {
  var options = {};

  // Build a list of current options.
  var previousOption = false;
  $(this.optionsElement).find('input.form-options-value').each(function() {
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
  var optionsText = Drupal.optionsElement.optionsToText(options);
  var defaultText = Drupal.optionsElement.defaultsToText(defaultValue);
  $(this.manualDefaultValueElement).val(defaultText);
  $(this.manualOptionsElement).val(optionsText).change();
}

/**
 * Several maintenance routines to update all rows of the options element.
 *
 * - Disable options for optgroups if indented.
 * - Disable add and delete links if indented.
 * - Match the default value radio button value to the key of the text element.
 */
Drupal.optionsElement.prototype.updateOptionElements = function() {
  var self = this;
  var previousElement = false;

  $(this.optionsElement).find('tr').each(function() {
    // Update the elements key if matching the key and value.
    if (self.keyType == 'associative') {
      var optionValue = $(this).find('input.form-options-value').val();
      $(this).find('input.option-key').val(optionValue);
    }
    // Match the default value checkbox/radio button to the option's key.
    var optionKey =$(this).find('input.option-key').val();
    $(this).find('input.option-default').val(optionKey);

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
  console.log(currentOption);
  var self = this;
  var newOption = $(currentOption).clone()
    .find('input.form-options-value').val('').end()
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
  $('input.form-options-value', newOption).keyup(function(e) {
    self.pendingUpdate(e);
  });

  this.updateOptionElements();
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
    this.options = Drupal.optionsElement.optionsFromText(this.manualOptionsElement.value);
    this.defaultValue = Drupal.optionsElement.defaultsFromText(this.manualDefaultValueElement.value, this.multiple);
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
Drupal.optionsElement.optionsToText = function(options) {
  var output = '';
  var previousKey = false;

  for (var key in options) {
    // Convert groups.
    if (typeof(options[key]) == 'object') {
      output += '<' + key + '>' + "\n";
      for (var subkey in options[key]) {
        output += subkey + '|' + options[key][subkey] + "\n";
      }
      previousKey = key;
    }
    // Typical key|value pairs.
    else {
      // Exit out of any groups.
      if (typeof(options[previousKey]) == 'object') {
        output += "<>\n";
      }
      // Skip empty rows.
      if (options[key]) {
        output += key + '|' + options[key] + "\n";
        previousKey = key;
      }
    }
  }

  return output;
};

/**
 * Given a text string, convert it to an object.
 */
Drupal.optionsElement.optionsFromText = function(text, flat) {
  var flat = flat || false; // Default to false if not specified.
  var options = {};
  var rows = text.match(/^.+$/mg);
  var group = false;
  for (var n in rows) {
    var option = rows[n].replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().

    var matches = {};
    // Check if this row is a group.
    if (!flat && (matches = option.match(/^\<([^>]*)\>$/))) {
      if (matches[1] === '') {
        group = false;
      }
      else {
        group = matches[1];
        options[group] = {};
      }
    }
    // Check if this row is a key|value pair.
    else if (matches = option.match(/^([^|]+)\|(.*)$/)) {
      var key = matches[1];
      var value = matches[2];
      if (group !== false) {
        options[group][key] = value;
      }
      else {
        options[key] = value;
      }
    }
    // Check if this row is a straight value.
    else {
      if (group !== false) {
        options[group][option] = option
      }
      else {
        options[option] = option;
      }
    }
  }

  return options;
}

/**
 * Convert from a default value to a text string.
 *
 * @param defaults
 *   The string or array of defaults being converted to default values.
 */
Drupal.optionsElement.defaultsToText = function(defaults) {
  var text = defaults;

  if (typeof(defaults) == 'array') {
    text = defaults.join(', ');
  }

  return text;
}


/**
 * Convert text from a string to an array (or string) of default values.
 *
 * @param text
 *   The string being converted to default values.
 * @param multiple
 *   The type of values being parsed. If true, the returned value will be an
 *   array. Otherwise the returned value will be a string.
 */
Drupal.optionsElement.defaultsFromText = function(text, multiple) {
  var defaultValue = multiple ? [] : '';

  if (multiple) {
    defaultValue = text.split(',');
    for (var n in defaultValue) {
      defaultValue[n] = defaultValue[n].replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
    }
  }
  else {
    defaultValue = text.replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
  }

  return defaultValue;
}

/**
 * Theme function for creating a new options element.
 *
 * @param optionsElement
 *   An options element object.
 */
Drupal.theme.prototype.optionsElement = function(optionsElement) {
  var output = '';
  var options = optionsElement.options;
  var defaultValue = optionsElement.defaultValue;
  var defaultType = optionsElement.multiple ? 'checkbox' : 'radio';

  // Helper function to check if an option is selected.
  function isSelected(option) {
    if (optionsElement.multiple && defaultValue.indexOf(option) != -1) {
      return true;
    }
    else if (!optionsElement.multiple && defaultValue == option) {
      return true;
    }
    return false;
  }

  // Helper function to print out a single draggable option row.
  function tableDragRow(key, value, parent_key, indent, status) {
    var output = '';
    output += '<tr class="draggable">'
    output += '<td>';
    for (var n = 0; n < indent; n++) {
      output += Drupal.theme('tableDragIndentation');
    }
    output += '<input type="hidden" class="option-key" value="' + key + '" />';
    output += '<input type="hidden" class="option-parent" value="' + parent_key + '" />';
    output += '<input type="hidden" class="option-depth" value="' + indent + '" />';
    output += '<input type="' + defaultType + '" name="' + optionsElement.identifier + '-default" class="form-radio option-default" value="' + key + '"' + (status == 'checked' ? ' checked="checked"' : '') + (status == 'disabled' ? ' disabled="disabled"' : '') + ' />';
    output += '<input class="form-text form-options-value" type="text" value="' + value + '" />';
    output += '<a class="add" title="' + Drupal.t('Add new option') + '" href="#"' + (status == 'disabled' ? ' style="display: none"' : '') + '><span class="add">' + Drupal.t('Add') + '</span></a>';
    output += '<a class="remove" title="' + Drupal.t('Remove option') + '" href="#"' + (status == 'disabled' ? ' style="display: none"' : '') + '><span class="remove">' + Drupal.t('Remove') + '</span></a>';
    output += '</td>';
    output += '</tr>';
    return output;
  }

  output += '<div class="options-widget">';
  output += '<table id="' + optionsElement.identifier + '">';
  output += '<tbody>';

  for (var key in options) {
    if (typeof(options[key]) == 'object') {
      output += tableDragRow(key, key, 0, 0, 'disabled');
      for (var subkey in options[key]) {
        output += tableDragRow(subkey, options[key][subkey], key, 1, (isSelected(subkey) ? 'checked' : false));
      }
    }
    else {
      output += tableDragRow(key, options[key], 0, 0, (isSelected(key) ? 'checked' : false));
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
