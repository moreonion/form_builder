// $Id$

/**
 * @file
 * Add JavaScript behaviors for the "options" form element type.
 */
Drupal.behaviors.optionsField = function(context) {
  $('fieldset.form-options', context).each(function() {
    var options = textToOptions($(this).find('textarea').val());
    var defaultValue = $(this).find('input').val();
  });

  /**
   * Given an object of options, convert it to a text string.
   */
  function optionsToText(options) {
    var output = '';
    for (var key in options) {
      // Convert groups.
      if (typeof(options[key]) == 'object') {
        output += '<' + key + '>' + "\n";
        for (var subkey in options[key]) {
          output += subkey + '|' + options[key][subkey] + "\n";
        }
      }
      // Typical key|value pairs.
      else {
        output += key + '|' + options[key] + "\n";
      }
    }

    return output;
  }

  /**
   * Given a text string, convert it to an object.
   */
  function textToOptions(text, flat) {
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

};