<?php

/**
 * @file form_builder.api.inc
 * Universally used API functions within the Form builder module.
 */

/**
 * Get a list of all properties that are supported within a form type.
 *
 * @deprecated Deprecated since 1.8 and will be removed in 2.0.
 *   Use the Form / Element or Property classes instead of the info-arrays.
 */
function form_builder_get_properties($form_type, $reset = FALSE) {
  return FormBuilderLoader::instance()->getPropertyInfo($form_type, $reset);
}

/**
 * Get a list of all properties that are supported by a particular form type.
 *
 * @deprecated Deprecated since 1.8 and will be removed in 2.0.
 *   Use the Form / Element or Property classes instead of the info-arrays.
 */
function form_builder_get_form_type($form_type = NULL, $reset = FALSE) {
  return FormBuilderLoader::instance()->getElementTypeInfo($form_type);
}

/**
 * Given an element type, return properties that are supported by Form builder.
 *
 * @deprecated Deprecated since 1.8 and will be removed in 2.0.
 *   Use the FormBuilderElement class instead of the info-arrays.
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
  $properties = FormBuilderLoader::instance()->getPropertyInfo($form_type);
  $elements = FormBuilderLoader::instance()->getElementTypeInfo($form_type);
  return array_intersect_key(
    $properties,
    array_flip($elements[$element_type]['properties'])
  );
}

/**
 * Get a list of properties that are supported in any way by an element.
 *
 * This returns a list of all supported properties within an element, even
 * if some of those properties do not have an interface for editing or are
 * only used internally by the module providing the form type this element
 * is being saved in.
 *
 * @deprecated Deprecated since 1.8 and will be removed in 2.0.
 *   Use the FormBuilderElement class instead of the info-arrays.
 *
 * @param $form_type
 *   The type of form being edited (profile, node, webform, etc.)
 * @param $element
 *   A standard FAPI element whose properties are being checked.
 * @return
 *   A non-indexed list of properties that may be saved for this element.
 * */
function form_builder_get_saveable_properties($form_type, $element) {
  $element_type = $element['#form_builder']['element_type'];
  $info = FormBuilderLoader::instance()->getElementTypeInfo($form_type);
  return $info[$element_type]['properties'];
}

/**
 * Function to retrieve a single element within a form structure.
 *
 * If needing to retreive multiple elements at once, use
 * form_builder_get_elements().
 *
 * @param $form
 *   A complete hierarchical FAPI structure.
 * @param $element_id
 *   The unique identifier for an element that is to be retrieved.
 * @return
 *   A single Form API element array.
 *
 * @see form_builder_get_elements().
 *
 * @deprecated Use FormBuilderFormInterface::getElement() instead.
 *   Will be removed in 2.0.
 */
function form_builder_get_element(&$form, $element_id) {
  return FormBuilderFormBase::fromArray($form)->getElementArray($element_id);
}

/**
 * Retrieve multiple elements within a form structure.
 *
 * @param $form
 *   A complete hierarchical FAPI structure.
 * @param $element_ids
 *   An array of unique identifiers for elements that are to be retreived. These
 *   identifiers match against the special property
 *   "#form_builder['element_id']", which is not available in normal FAPI
 *   structures. It must be added by the respective module that is providing
 *   support for a certain field type.
 *
 *   For example, CCK provides a unique identifier for each field such as
 *   "field_my_name". This field name must be added to the form array as
 *   #form_builder['element_id'] = 'field_my_name' in CCK's implementation of
 *   hook_form_builder_load().
 * @return
 *   A single Form API element array.
 */
function form_builder_get_elements(&$form, $element_ids) {
  return FormBuilderFormBase::fromArray($form)->getFormArray();
}

/**
 * Set an element within a form structure.
 *
 * @deprecated Deprecated since 1.8 and will be removed in 2.0.
 *   Use the FormBuilderElement class instead of the info-arrays.
 *
 * @return
 *   TRUE if an element was updated, FALSE if it was not found.
 */
function form_builder_set_element(&$form, $element, &$entire_form = NULL, $parent_id = FORM_BUILDER_ROOT) {
  $form_obj = FormBuilderFormBase::fromArray($form);
  $r = $form_obj->setElementArray($element);
  $form = $form_obj->getFormArray();
  return $r;
}

/**
 * Unset an element within a form structure.
 *
 * @deprecated Deprecated since 1.8 and will be removed in 2.0.
 *   Use the FormBuilderElement class instead of the info-arrays.
 */
function form_builder_unset_element(&$form, $element_id) {
  $form_obj = FormBuilderFormBase::fromArray($form);
  $r = $form_obj->unsetElement($element_id);
  $form = $form_obj->getFormArray();
  return $r;
}

/**
 * Recursive function to check if an element exists at all within a form.
 *
 * @deprecated Use FormBuilderFormInterface::getElementIds() instead.
 */
function form_builder_get_element_ids($form) {
  return FormBuilderFormBase::fromArray($form)->getElementIds();
}

/**
 * Recursive function to get the types of all element within a form.
 *
 * @deprecated Use FormBuilderFormInterface::getElementTypes() instead.
 */
function form_builder_get_element_types($form) {
  return FormBuilderFormBase::fromArray($form)->getElementTypes();
}

/**
 * Loader function to retrieve a form builder configuration array.
 *
 * @deprecated Use FormBuilderLoader::fromStorage() instead.
 *
 * @param $form_type
 *   The type of form being edited. Usually the name of the providing module.
 * @param $form_id
 *   The unique identifier for the form being edited with the type.
 */
function form_builder_load_form($form_type, $form_id) {
  return FormBuilderLoader::instance()
    ->fromStorage($form_type, $form_id)
    ->getFormArray();
}

/**
 * Execute the save methods for a form array.
 *
 * @deprecated Use FormBuilderFormInterface::saveToStorage().
 */
function form_builder_save_form(&$form, $form_type, $form_id) {
  FormBuilderFormBase::fromArray($form)->saveToStorage();
}

/**
 * Helper function to add default #form_builder properties to a form.
 *
 * @todo implement this in the form object.
 */
function form_builder_add_default_properties($form, $form_type, $key = NULL, $parent_id = FORM_BUILDER_ROOT) {
  $form_type_fields = FormBuilderLoader::instance()->getElementTypeInfo($form_type);

  // Add the form type to the form so we can use it for later object creation.
  if ($parent_id === FORM_BUILDER_ROOT) {
    $form['#form_builder']['form_type'] = $form_type;
  }

  // Add properties to this element.
  if (isset($form['#form_builder']['element_id'])) {
    $element_id = $form['#form_builder']['element_id'];

    // Add a #key property.
    $form['#key'] = isset($form['#key']) ? $form['#key'] : $key;

    // Add a #form_builder['parent_id'] property.
    $form['#form_builder']['parent_id'] = $parent_id;
    $parent_id = $element_id;

    // Set defaults based on the form type.
    if (isset($form_type_fields[$element_id]) && $form_type_fields[$element_id]['unique']) {
      $form['#form_builder']['unique'] = TRUE;
      $form['#form_builder']['element_type'] = isset($form['#form_builder']['element_type']) ? $form['#form_builder']['element_type'] : $element_id;
      $settings = $form_type_fields[$element_id];
    }
    else {
      $form['#form_builder']['element_type'] = isset($form['#form_builder']['element_type']) ? $form['#form_builder']['element_type'] : $form['#type'];
      if (isset($form_type_fields[$form['#form_builder']['element_type']])) {
        $settings = $form_type_fields[$form['#form_builder']['element_type']];
      }
      else {
        // If the type cannot be found, prevent editing of this field.
        unset($form['#form_builder']);
        return $form;
      }
    }

    // Set defaults for configurable and removable.
    if (!isset($form['#form_builder']['configurable'])) {
      $form['#form_builder']['configurable'] = isset($settings['configurable']) ? $settings['configurable'] : TRUE;
    }
    if (!isset($form['#form_builder']['removable'])) {
      $form['#form_builder']['removable'] = isset($settings['removable']) ? $settings['removable'] : TRUE;
    }
  }

  // Recurse into sub-elements.
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#form_builder']['element_id'])) {
      $form[$key] = form_builder_add_default_properties($form[$key], $form_type, $key, $parent_id);
    }
  }

  return $form;
}

/**
 * Helper function to sort elements by 'weight' and 'title'.
 */
function _form_builder_sort($a, $b) {
  $a_weight = (is_array($a) && isset($a['weight'])) ? $a['weight'] : 0;
  $b_weight = (is_array($b) && isset($b['weight'])) ? $b['weight'] : 0;
  if ($a_weight == $b_weight) {
    if (!isset($b['title'])) {
      return -1;
    }
    if (!isset($a['title'])) {
      return 1;
    }
    return strcasecmp($a['title'], $b['title']);
  }
  return ($a_weight < $b_weight) ? -1 : 1;
}
