<?php
// $Id$

/**
 * @file form_builder.api.inc
 * Universally used API functions within the Form builder module.
 */

/**
 * Get a list of all properties that are supported globally.
 */
function form_builder_get_properties($reset = FALSE) {
  static $properties;

  if (!isset($properties) || $reset) {
    // Get the list of all properties for all elements.
    $properties = module_invoke_all('form_builder_properties');
    drupal_alter('form_builder_properties', $properties);
  }

  return $properties;
}

/**
 * Get a list of all properties that are supported by a particular field type.
 */
function form_builder_get_form_type($form_type = NULL, $reset = FALSE) {
  static $types;

  if (!isset($types) || $reset) {
    $types = array();
    // Get the list of all properties for all elements.
    $types = module_invoke_all('form_builder_types');

    // Add default values for undefined properties.
    foreach ($types as $type_key => &$type) {
      foreach ($type as $field_key => &$field) {
        // All fields must support key and weight.
        if (!in_array('weight', $field['properties'])) {
          $field['properties'][] = 'weight';
        }
        if (!in_array('key', $types[$type_key][$field_key]['properties'])) {
          $field['properties'][] = 'key';
        }

        $field['unique'] = isset($field['unique']) && $field['unique'];
        $field['default']['#form_builder']['configurable'] = isset($field['default']['#form_builder']['configurable']) ? $field['default']['#form_builder']['configurable'] : TRUE;
        $field['default']['#form_builder']['removable'] = isset($field['default']['#form_builder']['removable']) ? $field['default']['#form_builder']['removable'] : TRUE;
      }
    }

    drupal_alter('form_builder_types', $types);
  }

  return isset($form_type) ? $types[$form_type] : $types;
}

/**
 * Given an element type, return properties that are supported by Form builder.
 *
 * @param $form_type
 *   The type of form being edited (profile, node, webform, etc.)
 * @param $element_type
 *   A the #type property of a FAPI element.
 * @param
 *   An associative array of properties supported for editing, keyed by
 *   the property name and containing an array of the form, submit, and
 *   validate functions (if any).
 */
function form_builder_get_element_properties($form_type, $element_type) {
  // Get the list of all properties for all elements.
  $properties = form_builder_get_properties();
  // Get the list of supported properties in this form type.
  $form_type = form_builder_get_form_type($form_type);

  $element_properties = array();
  if (isset($form_type[$element_type]['properties'])) {
    foreach ($form_type[$element_type]['properties'] as $property) {
      if (isset($properties[$property])) {
        $element_properties[$property] = $properties[$property];
      }
    }
  }

  return $element_properties;
}

/**
 * Get a list of properties that are supported in any way by an element.
 *
 * This returns a list of all supported properties within an element, even
 * if some of those properties do not have an interface for editing or are
 * only used internally by the module providing the form type this element
 * is being saved in.
 *
 * @param $form_type
 *   The type of form being edited (profile, node, webform, etc.)
 * @param $element
 *   A standard FAPI element whose properties are being checked.
 * @return
 *   A non-indexed list of properties that may be saved for this element.
 * */
function form_builder_get_saveable_properties($form_type, $element) {
  // Get the list of supported properties on each element.
  $form_type = form_builder_get_form_type($form_type);

  $saveable = array();
  if (isset($form_type[$element['#type']]['properties'])) {
    $saveable = $form_type[$element['#type']]['properties'];
  }

  return $saveable;
}

/**
 * Recursive function to retrieve an element within a form structure.
 *
 * @param $form
 *   A complete hierarchical FAPI structure.
 * @param $element_id
 *   The unique identifier for an element that is to be retreived. This matches
 *   against the special property "#form_builder['element_id']", which is not
 *   available in normal FAPI structures. It must be added by the respective
 *   module that is providing support for a certain field type.
 *
 *   For example, CCK provides a unique identifier for each field such as
 *   "field_my_name". This field name must be added to the form array as a
 *   #form_builder['element_id'] in CCK's implementation of
 *   hook_form_builder_load().
 * @return
 *   A single Form API element array.
 */
function form_builder_get_element(&$form, $element_id) {
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#form_builder']['element_id']) && $form[$key]['#form_builder']['element_id'] == $element_id) {
      return $form[$key];
    }
    if ($element = form_builder_get_element($form[$key], $element_id)) {
      return $element;
    }
  }
}

/**
 * Recursive function to set an element within a form structure.
 */
function form_builder_set_element(&$form, $element, &$entire_form = NULL, $parent_id = FORM_BUILDER_ROOT) {
  if (!isset($entire_form)) {
    $entire_form = &$form;
  }

  // Add new elements into the current parent.
  if (isset($element['#form_builder']['is_new']) && strcmp($element['#form_builder']['parent_id'], $parent_id) == 0) {
    unset($element['#form_builder']['is_new']);
    unset($element['#form_builder']['parent_id']);
    $new_key = $element['#key'];
    $form[$new_key] = $element;
    return;
  }

  foreach (element_children($form) as $key) {
    // Update an existing element if it lives in the current parent.
    if (isset($form[$key]['#form_builder']['element_id']) && $form[$key]['#form_builder']['element_id'] == $element['#form_builder']['element_id']) {

      // If the parent has changed, re-parent the element to a new fieldset.
      if (isset($element['#form_builder']['parent_id']) && strcmp($element['#form_builder']['parent_id'], $parent_id) != 0) {
        // Remove the current element from the form.
        unset($form[$key]);
        // Recurse again through the entire form to insert into the new position.
        $element['#form_builder']['is_new'] = TRUE;
        form_builder_set_element($entire_form, $element);
      }
      // Handle key changes and replace the existing element in place.
      elseif (isset($element['#key']) && $key != $element['#key']) {
        $new_key = $element['#key'];
        $index = array_search($key, $form);
        $before = array_slice($form, 0, $index + 1, TRUE);
        $after = array_slice($form, $index + 2, NULL, TRUE);
        $form = $before + array($new_key => $element) + $after;
        unset($form[$key]);
      }
      // Or, most common case scenario, just update the element, no key changes.
      else {
        $form[$key] = $element;
      }

      break;
    }

    // Recurse into this element to look for the target element.
    form_builder_set_element($form[$key], $element, $entire_form, $form[$key]['#form_builder']['element_id']);
  }
}

/**
 * Recursive function to unset an element within a form structure.
 */
function form_builder_unset_element(&$form, $element_id) {
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#form_builder']['element_id']) && $form[$key]['#form_builder']['element_id'] == $element_id) {
      unset($form[$key]);
      break;
    }
    form_builder_unset_element($form[$key], $element_id);
  }
}

/**
 * Recursive function to check if an element exists at all within a form.
 */
function form_builder_get_element_ids($form) {
  $element_ids = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#form_builder']['element_id'])) {
      $element_ids[] = $form[$key]['#form_builder']['element_id'];
    }
    $additional_ids = form_builder_get_element_ids($form[$key]);
    $element_ids = array_merge($element_ids, $additional_ids);
  }

  return $element_ids;
}